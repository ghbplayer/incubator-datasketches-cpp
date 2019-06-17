/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <kll_sketch.hpp>
#include <kll_helper.hpp>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <assert.h>

#ifdef KLL_VALIDATION

// This is to make sure the implementation matches exactly the reference implementation in OCaml.
// Conditional compilation is used because the implementation needs a few modifications:
// - switch from random choice to deterministic
// - a few methods to expose internals of the sketch

namespace datasketches {

uint32_t kll_next_offset; // to make kll_sketch deterministic

constexpr unsigned num_tests = 114;

const int64_t correct_results[num_tests * 7] = {
    0, 200, 180, 3246533, 1, 180, 1098352976109474698,
    1, 200, 198, 8349603, 1, 198, 686681527497651888,
    2, 200, 217, 676491, 2, 117, 495856134049157644,
    3, 200, 238, 3204507, 2, 138, 44453438498725402,
    4, 200, 261, 2459373, 2, 161, 719830627391926938,
    5, 200, 287, 5902143, 2, 187, 389303173170515580,
    6, 200, 315, 5188793, 2, 215, 985218890825795000,
    7, 200, 346, 801923, 2, 246, 589362992166904413,
    8, 200, 380, 2466269, 2, 280, 1081848693781775853,
    9, 200, 418, 5968041, 2, 318, 533825689515788397,
    10, 200, 459, 3230027, 2, 243, 937332670315558786,
    11, 200, 504, 5125875, 2, 288, 1019197831515566845,
    12, 200, 554, 4195571, 3, 230, 797351479150148224,
    13, 200, 609, 2221181, 3, 285, 451246040374318529,
    14, 200, 669, 5865503, 3, 345, 253851269470815909,
    15, 200, 735, 831703, 3, 411, 491974970526372303,
    16, 200, 808, 4830785, 3, 327, 1032107507126916277,
    17, 200, 888, 1356257, 3, 407, 215225420986342944,
    18, 200, 976, 952071, 3, 417, 600280049738270697,
    19, 200, 1073, 6729833, 3, 397, 341758522977365969,
    20, 200, 1180, 6017925, 3, 406, 1080227312339182949,
    21, 200, 1298, 4229891, 3, 401, 1092460534756675086,
    22, 200, 1427, 7264889, 4, 320, 884533400696890024,
    23, 200, 1569, 5836327, 4, 462, 660575800011134382,
    24, 200, 1725, 5950087, 4, 416, 669373957401387528,
    25, 200, 1897, 2692555, 4, 406, 607308667566496888,
    26, 200, 2086, 1512443, 4, 459, 744260340112029032,
    27, 200, 2294, 2681171, 4, 434, 199120609113802485,
    28, 200, 2523, 3726521, 4, 450, 570993497599288304,
    29, 200, 2775, 2695247, 4, 442, 306717093329516310,
    30, 200, 3052, 5751175, 5, 400, 256024589545754217,
    31, 200, 3357, 1148897, 5, 514, 507276662329207479,
    32, 200, 3692, 484127, 5, 457, 1082660223488175122,
    33, 200, 4061, 6414559, 5, 451, 620820308918522117,
    34, 200, 4467, 5587461, 5, 466, 121975084804459305,
    35, 200, 4913, 1615017, 5, 483, 152986529342916376,
    36, 200, 5404, 6508535, 5, 492, 858526451332425960,
    37, 200, 5944, 2991657, 5, 492, 624906434274621995,
    38, 200, 6538, 6736565, 6, 511, 589153542019036049,
    39, 200, 7191, 1579893, 6, 507, 10255312374117907,
    40, 200, 7910, 412509, 6, 538, 570863587164194186,
    41, 200, 8701, 1112089, 6, 477, 553100668286355347,
    42, 200, 9571, 1258813, 6, 526, 344845406406036297,
    43, 200, 10528, 1980049, 6, 508, 411846569527905064,
    44, 200, 11580, 2167127, 6, 520, 966876726203675488,
    45, 200, 12738, 1975435, 7, 561, 724125506920592732,
    46, 200, 14011, 4289627, 7, 560, 753686005174215572,
    47, 200, 15412, 5384001, 7, 494, 551637841878573955,
    48, 200, 16953, 2902685, 7, 560, 94602851752354802,
    49, 200, 18648, 4806445, 7, 562, 597672400688514221,
    50, 200, 20512, 2085, 7, 529, 417280161591969960,
    51, 200, 22563, 6375939, 7, 558, 11300453985206678,
    52, 200, 24819, 7837057, 7, 559, 283668599967437754,
    53, 200, 27300, 6607975, 8, 561, 122183647493325363,
    54, 200, 30030, 1519191, 8, 550, 1145227891427321202,
    55, 200, 33033, 808061, 8, 568, 71070843834364939,
    56, 200, 36336, 2653529, 8, 570, 450311772805359006,
    57, 200, 39969, 2188957, 8, 561, 269670427054904115,
    58, 200, 43965, 5885655, 8, 539, 1039064186324091890,
    59, 200, 48361, 6185889, 8, 574, 178055275082387938,
    60, 200, 53197, 208767, 9, 579, 139766040442973048,
    61, 200, 58516, 2551345, 9, 569, 322655279254252950,
    62, 200, 64367, 1950873, 9, 569, 101542216315768285,
    63, 200, 70803, 2950429, 9, 582, 72294008568551853,
    64, 200, 77883, 3993977, 9, 572, 299014330559512530,
    65, 200, 85671, 428871, 9, 585, 491351721800568188,
    66, 200, 94238, 6740849, 9, 577, 656204268858348899,
    67, 200, 103661, 2315497, 9, 562, 829926273188300764,
    68, 200, 114027, 5212835, 10, 581, 542222554617639557,
    69, 200, 125429, 4213475, 10, 593, 713339189579860773,
    70, 200, 137971, 2411583, 10, 592, 649651658985845357,
    71, 200, 151768, 5243307, 10, 567, 1017459402785275179,
    72, 200, 166944, 2468367, 10, 593, 115034451827634398,
    73, 200, 183638, 2210923, 10, 583, 365735165000548572,
    74, 200, 202001, 321257, 10, 591, 928479940794929153,
    75, 200, 222201, 8185105, 11, 600, 780163958693677795,
    76, 200, 244421, 6205349, 11, 598, 132454307780236135,
    77, 200, 268863, 3165901, 11, 600, 369824066179493948,
    78, 200, 295749, 2831723, 11, 595, 80968411797441666,
    79, 200, 325323, 464193, 11, 594, 125773061716381917,
    80, 200, 357855, 7499035, 11, 576, 994150328579932916,
    81, 200, 393640, 1514479, 11, 596, 111092193875842594,
    82, 200, 433004, 668493, 12, 607, 497338041653302784,
    83, 200, 476304, 3174931, 12, 606, 845986926165673887,
    84, 200, 523934, 914611, 12, 605, 354993119685278556,
    85, 200, 576327, 7270385, 12, 602, 937679531753465428,
    86, 200, 633959, 1956979, 12, 598, 659413123921208266,
    87, 200, 697354, 3137635, 12, 606, 874228711599628459,
    88, 200, 767089, 214923, 12, 608, 1077644643342432307,
    89, 200, 843797, 3084545, 13, 612, 79317113064339979,
    90, 200, 928176, 7800899, 13, 612, 357414065779796772,
    91, 200, 1020993, 6717253, 13, 615, 532723577905833296,
    92, 200, 1123092, 5543015, 13, 614, 508695073250223746,
    93, 200, 1235401, 298785, 13, 616, 34344606952783179,
    94, 200, 1358941, 4530313, 13, 607, 169924026179364121,
    95, 200, 1494835, 4406457, 13, 612, 1026773494313671061,
    96, 200, 1644318, 1540983, 13, 614, 423454640036650614,
    97, 200, 1808749, 7999631, 14, 624, 466122870338520329,
    98, 200, 1989623, 4295537, 14, 621, 609309853701283445,
    99, 200, 2188585, 7379971, 14, 622, 141739898871015642,
    100, 200, 2407443, 6188931, 14, 621, 22515080776738923,
    101, 200, 2648187, 6701239, 14, 619, 257441864177795548,
    102, 200, 2913005, 2238709, 14, 623, 867028825821064773,
    103, 200, 3204305, 5371075, 14, 625, 1110615471273395112,
    104, 200, 3524735, 7017341, 15, 631, 619518037415974467,
    105, 200, 3877208, 323337, 15, 633, 513230912593541122,
    106, 200, 4264928, 6172471, 15, 628, 885861662583325072,
    107, 200, 4691420, 5653803, 15, 633, 754052473303005204,
    108, 200, 5160562, 1385265, 15, 630, 294993765757975100,
    109, 200, 5676618, 4350899, 15, 617, 1073144684944932303,
    110, 200, 6244279, 1272235, 15, 630, 308982934296855020,
    111, 200, 6868706, 1763939, 16, 638, 356231694823272867,
    112, 200, 7555576, 3703411, 16, 636, 20043268926300101,
    113, 200, 8311133, 6554171, 16, 637, 121111429906734123
};

class kll_sketch_validation: public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(kll_sketch_validation);
  CPPUNIT_TEST(test_hash);
  CPPUNIT_TEST(test_make_input_array);
  CPPUNIT_TEST(validation);
  CPPUNIT_TEST_SUITE_END();

  void validation() {
    for (unsigned i = 0; i < num_tests; i++) {
      assert (correct_results[7 * i] == i);
      unsigned k(correct_results[7 * i + 1]);
      unsigned n(correct_results[7 * i + 2]);
      unsigned stride(correct_results[7 * i + 3]);
      std::unique_ptr<int[]> input_array = make_input_array(n, stride);
      kll_sketch<float> sketch(k);
      kll_next_offset = 0;
      for (unsigned j = 0; j < n; j++) {
        sketch.update(input_array[j]);
      }
      unsigned num_levels = sketch.get_num_levels();
      unsigned num_samples = sketch.get_num_retained();
      int64_t hashed_samples = simple_hash_of_sub_array(sketch.get_items(), sketch.get_levels()[0], num_samples);
      std::cout << i;
      assert (correct_results[7 * i + 4] == num_levels);
      assert (correct_results[7 * i + 5] == num_samples);
      if (correct_results[7 * i + 6] == hashed_samples) {
        std::cout << " pass" << std::endl;
      } else {
        std::cout << " " << (correct_results[7 * i + 6]) << " != " << hashed_samples;
        std::cout << sketch;
        CPPUNIT_FAIL("fail");
      }
    }
  }

  static std::unique_ptr<int[]> make_input_array(unsigned n, unsigned stride) {
    assert (kll_helper::is_odd(stride));
    unsigned mask((1 << 23) - 1); // because library items are single-precision floats at the moment
    unsigned cur(0);
    std::unique_ptr<int[]> arr(new int[n]);
    for (unsigned i = 0; i < n; i++) {
      cur += stride;
      cur &= mask;
      arr[i] = cur;
    }
    return std::move(arr);
  }

  static int64_t simple_hash_of_sub_array(const float* arr, unsigned start, unsigned length) {
    int64_t multiplier(738219921); // an arbitrary odd 30-bit number
    int64_t mask60((1ULL << 60) - 1ULL);
    int64_t accum(0);
    for (unsigned i = start; i < start + length; i++) {
      accum += (int64_t) arr[i];
      accum *= multiplier;
      accum &= mask60;
      accum ^= accum >> 30;
    }
    return accum;
  }

  void test_hash() {
    float array[] = { 907500, 944104, 807020, 219921, 678370, 955217, 426885 };
    CPPUNIT_ASSERT_EQUAL(1141543353991880193LL, simple_hash_of_sub_array(array, 1, 5));
  }

  void test_make_input_array() {
    int expected_array[6] = { 3654721, 7309442, 2575555, 6230276, 1496389, 5151110 };
    auto array(make_input_array(6, 3654721));
    CPPUNIT_ASSERT_EQUAL(expected_array[0], array[0]);
    CPPUNIT_ASSERT_EQUAL(expected_array[1], array[1]);
    CPPUNIT_ASSERT_EQUAL(expected_array[2], array[2]);
    CPPUNIT_ASSERT_EQUAL(expected_array[3], array[3]);
    CPPUNIT_ASSERT_EQUAL(expected_array[4], array[4]);
    CPPUNIT_ASSERT_EQUAL(expected_array[5], array[5]);
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(kll_sketch_validation);

} /* namespace datasketches */

#endif
