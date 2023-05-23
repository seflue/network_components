//
// Created by seflue on 20.05.2023.
//

#ifndef NETWORK_COMPONENTS_USERDATAGENERATOR_H
#define NETWORK_COMPONENTS_USERDATAGENERATOR_H

#include "CoinOrDice.h"
#include <optional>
#include <string>
#include <vector>

namespace user_equipment {
class UserdataGenerator {
  public:
    explicit UserdataGenerator() { selector.setDistribution({1, 1, 1, 1, 100}); }
    std::optional<std::string> generate()
    {
        int selection = selector.getValue();
        if (selection < files_.size()) {
            auto file = files_[selection];
            if (file.empty())
                return std::nullopt;
            files_[selection] = "";
            return path_ + "/" + file;
        }
        else
            return std::nullopt;
    }

  private:
    std::string path_ = "testdata";
    std::vector<std::string> files_ = {"music.txt", "video.txt", "picture.txt", "text.txt"};
    utils::CoinOrDice<5> selector;
};
} // namespace user_equipment

#endif // NETWORK_COMPONENTS_USERDATAGENERATOR_H
