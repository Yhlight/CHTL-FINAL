#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "config/ConfigScanner.h"
#include "config/ConfigurationManager.h"

using namespace CHTL;

TEST_CASE("ConfigScanner correctly parses configuration blocks", "[ConfigScanner]")
{
    ConfigurationManager::Reset();

    SECTION("Basic configuration parsing")
    {
        std::string source = R"(
            [Configuration] {
                [Name: spanish]
                inherit = "heredar";
                style = "estilo";

                [Name: french]
                style = "style_francais";
            }
        )";

        ConfigScanner scanner(source);
        scanner.Scan();

        auto &configManager = ConfigurationManager::GetInstance();

        configManager.SetActiveConfiguration("spanish");
        REQUIRE(configManager.GetKeyword("inherit") == "heredar");
        REQUIRE(configManager.GetKeyword("style") == "estilo");
        REQUIRE(configManager.GetKeyword("text") == "text"); // fallback

        configManager.SetActiveConfiguration("french");
        REQUIRE(configManager.GetKeyword("style") == "style_francais");
        REQUIRE(configManager.GetKeyword("inherit") == "inherit"); // fallback
    }

    SECTION("Source with no configuration block")
    {
        std::string source = R"(
            @Element div {
                style {
                    color: #FFF;
                }
            }
        )";

        ConfigScanner scanner(source);
        scanner.Scan();

        auto &configManager = ConfigurationManager::GetInstance();
        // Should not have any effect, manager remains in default state
        REQUIRE(configManager.GetKeyword("style") == "style");
    }

    SECTION("Empty configuration block")
    {
        std::string source = "[Configuration] {}";

        ConfigScanner scanner(source);
        scanner.Scan();

        auto &configManager = ConfigurationManager::GetInstance();
        REQUIRE(configManager.GetKeyword("style") == "style");
    }

    SECTION("Configuration with extra whitespace")
    {
        std::string source = R"(
            [Configuration] {

                [Name:   whitespace_test  ]

                key1   =   "value1"   ;
                  key2 = "value2";
            }
        )";

        ConfigScanner scanner(source);
        scanner.Scan();

        auto &configManager = ConfigurationManager::GetInstance();
        configManager.SetActiveConfiguration("whitespace_test");
        REQUIRE(configManager.GetKeyword("key1") == "value1");
        REQUIRE(configManager.GetKeyword("key2") == "value2");
    }
}
