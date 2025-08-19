#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <misc/spaced_span.h>

SCENARIO("SpacedSpan correctly iterates over elements") {
    GIVEN("A SpacedSpan referring to an array with 3 members") {
        using Element                      = std::uint32_t;
        constexpr std::size_t NumElements  = 3;
        constexpr std::size_t SpaceBetween = 8;

        std::array<Element, NumElements> elements{};
        std::iota(elements.begin(), elements.end(), 0);

        std::array<std::uint8_t, NumElements * (sizeof(Element) + SpaceBetween)>
                       buffer{};
        Element const *BufferPtr{reinterpret_cast<Element const *>(&buffer)};

        for (std::size_t i = 0; i < NumElements; ++i) {
            std::memcpy(
                    buffer.data() + i * (sizeof(Element) + SpaceBetween),
                    &elements[i], sizeof(Element)
            );
        }

        engine::SpacedSpan<Element const, SpaceBetween> span{
                BufferPtr, elements.size()
        };

        THEN("The span's data pointer and size matches what it was given") {
            REQUIRE(span.get_data() == BufferPtr);
            REQUIRE(span.get_size() == elements.size());
        }

        AND_THEN("The byte size should be correct") {
            REQUIRE(span.get_size_bytes() ==
                    (sizeof(Element) + SpaceBetween) * elements.size());
        }

        AND_WHEN("We iterate over elements") {
            auto it  = span.begin();
            auto end = span.end();

            AND_THEN("The first element should be 0") {
                REQUIRE(*it == 0);

                AND_THEN("The second element should be 1") {
                    ++it;
                    REQUIRE(*it == 1);

                    AND_THEN("The third element should be 2") {
                        ++it;
                        REQUIRE(*it == 2);

                        AND_THEN("The iterator should be the end sentinel") {
                            REQUIRE(++it == end);
                        }
                    }
                }
            }
        }
    }
}
