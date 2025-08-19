#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <math/matrix.h>
#include <math/vec.h>

SCENARIO("getting rows of matrix") {
    GIVEN("A 2x2 matrix") {
        using El  = float;
        using Mat = engine::math::SquareMatrix<El, 2>;
        using Vec = engine::math::Vec<El, 2>;
        Vec constexpr vec1{1.f, 2.f};
        Vec constexpr vec2{3.f, 4.f};

        constexpr Mat matrix{vec1, vec2};

        WHEN("We get the rows") {
            auto const rows = matrix.get_rows();

            THEN("The first row should be correct") {
                CHECK(rows[0][0] == vec1[0]);
                CHECK(rows[0][1] == vec1[1]);
            }

            THEN("The second row should be correct") {
                CHECK(rows[1][0] == vec2[0]);
                CHECK(rows[1][1] == vec2[1]);
            }
        }
    }
}

SCENARIO("getting cols of a matrix") {
    GIVEN("A 2x2 matrix") {
        using El  = float;
        using Mat = engine::math::SquareMatrix<El, 2>;
        using Vec = engine::math::Vec<El, 2>;
        Vec constexpr vec1{1.f, 2.f};
        Vec constexpr vec2{3.f, 4.f};

        constexpr Mat matrix{vec1, vec2};

        WHEN("We get the cols") {
            auto const cols = matrix.get_cols();

            THEN("The first col should be correct") {
                CHECK(cols[0].cbegin()[0] == vec1[0]);
                CHECK(cols[0].cbegin()[1] == vec2[0]);
            }

            THEN("The second col should be correct") {
                CHECK(cols[1].cbegin()[0] == vec1[1]);
                CHECK(cols[1].cbegin()[1] == vec2[1]);
            }
        }
    }
}

SCENARIO("2x2 matrix binary multiplication") {
    constexpr std::size_t Dimensions = 2;
    using El                         = float;
    using Mat = engine::math::SquareMatrix<El, Dimensions>;
    using Vec = engine::math::Vec<El, Dimensions>;

    GIVEN("Two 2x2 matrices") {
        Mat constexpr mat1{Vec{1.f, 2.f}, Vec{3.f, 4.f}};
        Mat constexpr mat2{Vec{5.f, 6.f}, Vec{7.f, 8.f}};


        WHEN("We multiply them") {
            Mat const result = mat1 * mat2;

            THEN("The result should be correct") {
                CHECK(result.get_vec(0) == Vec{19.f, 22.f});
                CHECK(result.get_vec(1) == Vec{43.f, 50.f});
            }
        }
    }
}

SCENARIO("3x3 matrix binary multiplication") {
    constexpr std::size_t Dimensions = 3;
    using El                         = float;
    using Mat = engine::math::SquareMatrix<El, Dimensions>;
    using Vec = engine::math::Vec<El, Dimensions>;

    GIVEN("Two 3x3 matrices") {
        Mat constexpr mat1{
                Vec{1.f, 2.f, 3.f}, Vec{4.f, 5.f, 6.f}, Vec{7.f, 8.f, 9.f}
        };
        Mat constexpr mat2{
                Vec{9.f, 8.f, 7.f}, Vec{6.f, 5.f, 4.f}, Vec{3.f, 2.f, 1.f}
        };

        WHEN("We multiply them") {
            Mat const result = mat1 * mat2;

            THEN("The result should be correct") {
                CHECK(result.get_vec(0) == Vec{30.f, 24.f, 18.f});
                CHECK(result.get_vec(1) == Vec{84.f, 69.f, 54.f});
                CHECK(result.get_vec(2) == Vec{138.f, 114.f, 90.f});
            }
        }
    }
}

SCENARIO("Translation matrix") {
    constexpr std::size_t Dimensions = 4;
    using El                         = float;
    using Mat = engine::math::SquareMatrix<El, Dimensions>;

    GIVEN("An identity matrix") {
        WHEN("We translate it") {
            THEN("The result should be correct") {
                Mat trans_mat{};
                trans_mat.translate(engine::math::Vec<El, 3>{10.f, 20.f, 30.f});

                auto const row0 = trans_mat.get_row(0);
                std::array constexpr expected_row0{1.f, 0.f, 0.f, 10.f};
                REQUIRE(std::equal(
                        row0.cbegin(), row0.cend(), expected_row0.cbegin()
                ));

                auto const row1 = trans_mat.get_row(1);
                std::array constexpr expected_row1{0.f, 1.f, 0.f, 20.f};
                REQUIRE(std::equal(
                        row1.cbegin(), row1.cend(), expected_row1.cbegin()
                ));

                auto const row2 = trans_mat.get_row(2);
                std::array constexpr expected_row2{0.f, 0.f, 1.f, 30.f};
                REQUIRE(std::equal(
                        row2.cbegin(), row2.cend(), expected_row2.cbegin()
                ));

                auto const row3 = trans_mat.get_row(3);
                std::array constexpr expected_row3{0.f, 0.f, 0.f, 1.f};
                REQUIRE(std::equal(
                        row3.cbegin(), row3.cend(), expected_row3.cbegin()
                ));
            }
        }
    }
}
