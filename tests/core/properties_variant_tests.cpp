// Copyright 2018 The Darwin Neuroevolution Framework Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <core/properties.h>
#include <core/utils.h>
#include <tests/testcase_output.h>
#include <third_party/gtest/gtest.h>

#include <third_party/json/json.h>
using json = nlohmann::json;

#include <stdio.h>
#include <string>
#include <vector>
using namespace std;

namespace properties_variant_tests {

// the variant tag enum
enum class VariantTag {
  Empty,
  Basic,
  Extra,
};

inline auto customStringify(core::TypeTag<VariantTag>) {
  static auto stringify = new core::StringifyKnownValues<VariantTag>{
    { VariantTag::Empty, "empty" },
    { VariantTag::Basic, "basic" },
    { VariantTag::Extra, "extra" },
  };
  return stringify;
}

// variant cases

struct EmptyProperties : public core::PropertySet {};

struct BasicProperties : public core::PropertySet {
  PROPERTY(bool_flag, bool, false, "A boolean flag");
  PROPERTY(max_value, double, 100.0, "Maximum value");
};

struct ExtraProperties : public core::PropertySet {
  PROPERTY(values, vector<float>, {}, "List of values");
  PROPERTY(scale, float, 1.0f, "Scale factor");
  PROPERTY(name, string, "Darwin", "Name");
};

// a test variant type
struct TestVariant : public core::PropertySetVariant<VariantTag> {
  CASE(VariantTag::Empty, empty, EmptyProperties);
  CASE(VariantTag::Basic, basic, BasicProperties);
  CASE(VariantTag::Extra, extra, ExtraProperties);
};

// a property with a variant member
struct TestProperties : public core::PropertySet {
  PROPERTY(start_value, int, 0, "Just a regular property");
  VARIANT(test_variant, TestVariant, VariantTag::Basic, "A variant property");
  VARIANT(another_variant, TestVariant, VariantTag::Empty, "Another variant property");
};

TEST(PropertiesVariantTest, CopyFrom) {
  TestVariant src_variant;
  src_variant.basic.bool_flag = true;
  src_variant.basic.max_value = 5000;
  src_variant.extra.name = "Foo";
  src_variant.selectCase(VariantTag::Basic);

  TestVariant dst_variant;
  dst_variant.copyFrom(src_variant);
  
  EXPECT_EQ(dst_variant.tag(), VariantTag::Basic);
  EXPECT_EQ(dst_variant.basic.bool_flag, true);
  EXPECT_EQ(dst_variant.basic.max_value, 5000);
  EXPECT_EQ(dst_variant.extra.name, "Foo");
}

TEST(PropertiesVariantTest, EmbeddedCopyFrom) {
  TestProperties src_properties;
  src_properties.start_value = 1000;
  src_properties.test_variant.basic.bool_flag = true;
  src_properties.test_variant.basic.max_value = 5000;
  src_properties.test_variant.extra.name = "Foo";
  src_properties.test_variant.selectCase(VariantTag::Basic);
  src_properties.another_variant.basic.max_value = 12345;
  src_properties.another_variant.extra.scale = 5.0f;
  src_properties.another_variant.selectCase(VariantTag::Extra);

  TestProperties dst_properties;
  dst_properties.copyFrom(src_properties);
  
  EXPECT_EQ(dst_properties.start_value, 1000);
  EXPECT_EQ(dst_properties.test_variant.tag(), VariantTag::Basic);
  EXPECT_EQ(dst_properties.test_variant.basic.bool_flag, true);
  EXPECT_EQ(dst_properties.test_variant.basic.max_value, 5000);
  EXPECT_EQ(dst_properties.test_variant.extra.name, "Foo");
  EXPECT_EQ(dst_properties.another_variant.tag(), VariantTag::Extra);
  EXPECT_EQ(dst_properties.another_variant.basic.max_value, 12345);
  EXPECT_EQ(dst_properties.another_variant.extra.scale, 5.0f);
}

TEST(PropertiesVariantTest, ToJson) {
  core_test::TestCaseOutput output;

  TestVariant test_variant;
  test_variant.selectCase(VariantTag::Basic);

  auto json_obj = test_variant.toJson();
  auto json_str = json_obj.dump(2);
  fprintf(output, "%s", json_str.c_str());
}

TEST(PropertiesVariantTest, EmbeddedToJson) {
  core_test::TestCaseOutput output;

  TestProperties test_properties;

  auto json_obj = test_properties.toJson();
  auto json_str = json_obj.dump(2);
  fprintf(output, "%s", json_str.c_str());
}

template <class VARIANT>
static void printVariant(FILE* file, const VARIANT& variant) {
  auto property_set = variant.activeCase();
  fprintf(file, "active tag: %s\n", core::toString(variant.tag()).c_str());
  for (const auto& property : property_set->properties()) {
    fprintf(file, "  %s = '%s'\n", property->name().c_str(), property->value().c_str());
  }
  fprintf(file, "---\n");
}

TEST(PropertiesVariantTest, FromJson) {
  core_test::TestCaseOutput output;

  const string json_str = R"(
    {
      "empty": {},
      "unrecognized": {
        "bool_flag": "true",
        "max_value": "50000"
      },
      "extra": {
        "name": "Lemo",
        "scale": "2.5",
        "values": "{}"
      },
      "tag": "extra"
    })";

  TestVariant test_variant;

  test_variant.fromJson(json::parse(json_str));
  printVariant(output, test_variant);

  test_variant.selectCase(VariantTag::Empty);
  printVariant(output, test_variant);

  test_variant.selectCase(VariantTag::Basic);
  printVariant(output, test_variant);
}

TEST(PropertiesVariantTest, EmbeddedJsonRoundtrip) {
  TestProperties src_properties;
  src_properties.start_value = 1000;
  src_properties.test_variant.basic.bool_flag = true;
  src_properties.test_variant.basic.max_value = 5000;
  src_properties.test_variant.extra.name = "Foo";
  src_properties.test_variant.selectCase(VariantTag::Basic);
  src_properties.another_variant.basic.max_value = 12345;
  src_properties.another_variant.extra.scale = 5.0f;
  src_properties.another_variant.selectCase(VariantTag::Extra);
  
  TestProperties dst_properties;
  dst_properties.fromJson(src_properties.toJson());
  
  EXPECT_EQ(dst_properties.start_value, 1000);
  EXPECT_EQ(dst_properties.test_variant.tag(), VariantTag::Basic);
  EXPECT_EQ(dst_properties.test_variant.basic.bool_flag, true);
  EXPECT_EQ(dst_properties.test_variant.basic.max_value, 5000);
  EXPECT_EQ(dst_properties.test_variant.extra.name, "Foo");
  EXPECT_EQ(dst_properties.another_variant.tag(), VariantTag::Extra);
  EXPECT_EQ(dst_properties.another_variant.basic.max_value, 12345);
  EXPECT_EQ(dst_properties.another_variant.extra.scale, 5.0f);
}

}  // namespace properties_variant_tests
