#include "ui/TeamPanel.hpp"
#include "utils/ResourceManager.hpp"
#include "utils/Logger.hpp"
#include <sstream>
#include <iomanip>
namespace zappy {
TeamPanel::TeamPanel()
    : visible_(false)
    , backgroundColor_(0, 0, 0, 180)
    , borderColor_(100, 100, 100, 255)
    , textColor_(255, 255, 255, 255)
    , progressBackColor_(50, 50, 50, 255)
    , winningTeamColor_(255, 215, 0, 255)
    , padding_(10.0f)
    , lineHeight_(16.0f)
    , sectionSpacing_(20.0f) 
{
}

bool TeamPanel::initialize(const sf::FloatRect& area) 
{
    area_ = area;
    font_ = ResourceManager::getInstance().getFont("default");
    background_.setSize(sf::Vector2f(area_.width, area_.height));
    background_.setPosition(area_.left, area_.top);
    background_.setFillColor(backgroundColor_);
    border_.setSize(sf::Vector2f(area_.width, area_.height));
    border_.setPosition(area_.left, area_.top);
    border_.setFillColor(sf::Color::Transparent);
    border_.setOutlineThickness(1.0f);
    border_.setOutlineColor(borderColor_);
    titleText_.setFont(font_);
    titleText_.setString("Teams");
    titleText_.setCharacterSize(14);
    titleText_.setFillColor(textColor_);
    titleText_.setPosition(area_.left + padding_, area_.top + padding_);
    LOG_DEBUG("TeamPanel initialized");
    return true;
}

void TeamPanel::update(float deltaTime, const World& world)
{
    (void)deltaTime;
    if (!visible_) return;
    teams_.clear();
    for (const auto& [teamId, team] : world.getTeams()) {
        TeamDisplayInfo info;
        info.name = team->getName();
        info.color = team->getColor();
        info.playerCount = 0;
        info.eggCount = 0;
        info.victoryProgress = 0.0f;
        info.isWinning = false;
        std::fill(info.levelCounts.begin(), info.levelCounts.end(), 0);
        std::fill(info.resources.begin(), info.resources.end(), 0);
        for (const auto& [playerId, player] : world.getPlayers()) {
            if (player->getTeamId() == teamId) {
                info.playerCount++;
                int level = player->getLevel();
                if (level >= 1 && level <= 8) {
                    info.levelCounts[level - 1]++;
                }
            }
        }
        for (const auto& [eggId, egg] : world.getEggs()) {
            if (egg->getTeamId() == teamId) {
                info.eggCount++;
            }
        }
        info.victoryProgress = static_cast<float>(info.levelCounts[7]) / 6.0f;
        info.isWinning = info.victoryProgress >= 1.0f;
        teams_.push_back(info);
    }
}

void TeamPanel::render(sf::RenderTarget& target) 
{
    if (!visible_) return;
    renderBackground(target);
    renderTeamList(target);
}

void TeamPanel::resize(const sf::FloatRect& area) 
{
    area_ = area;
    background_.setSize(sf::Vector2f(area_.width, area_.height));
    background_.setPosition(area_.left, area_.top);
    border_.setSize(sf::Vector2f(area_.width, area_.height));
    border_.setPosition(area_.left, area_.top);
    titleText_.setPosition(area_.left + padding_, area_.top + padding_);
}

void TeamPanel::setVisible(bool visible) 
{
    visible_ = visible;
}

bool TeamPanel::isVisible() const 
{
    return visible_;
}

void TeamPanel::setSelectedTeam(const std::string& teamName) 
{
    selectedTeam_ = teamName;
}

void TeamPanel::clearSelection() 
{
    selectedTeam_.clear();
}

void TeamPanel::renderBackground(sf::RenderTarget& target) 
{
    target.draw(background_);
    target.draw(border_);
    target.draw(titleText_);
}

void TeamPanel::renderTeamList(sf::RenderTarget& target) 
{
    float y = area_.top + padding_ + 30.0f;
    for (const auto& teamInfo : teams_) {
        sf::RectangleShape colorIndicator;
        colorIndicator.setSize(sf::Vector2f(12.0f, 12.0f));
        colorIndicator.setPosition(area_.left + padding_, y + 2.0f);
        colorIndicator.setFillColor(teamInfo.color);
        target.draw(colorIndicator);
        sf::Text teamNameText;
        teamNameText.setFont(font_);
        teamNameText.setString(teamInfo.name);
        teamNameText.setCharacterSize(12);
        teamNameText.setFillColor(textColor_);
        teamNameText.setPosition(area_.left + padding_ + 20.0f, y);
        target.draw(teamNameText);
        if (teamInfo.name == selectedTeam_) {
            sf::RectangleShape highlight;
            highlight.setSize(sf::Vector2f(area_.width - 2 * padding_, sectionSpacing_));
            highlight.setPosition(area_.left + padding_, y - 2.0f);
            highlight.setFillColor(winningTeamColor_);
            target.draw(highlight);
        }
        y += lineHeight_;
        std::ostringstream oss;
        oss << "Players: " << teamInfo.playerCount << " | Eggs: " << teamInfo.eggCount;
        sf::Text statsText;
        statsText.setFont(font_);
        statsText.setString(oss.str());
        statsText.setCharacterSize(10);
        statsText.setFillColor(sf::Color(200, 200, 200, 255));
        statsText.setPosition(area_.left + padding_ + 20.0f, y);
        target.draw(statsText);
        y += lineHeight_;
        oss.str("");
        oss << "Levels: ";
        for (size_t i = 0; i < teamInfo.levelCounts.size(); ++i) {
            if (i > 0) oss << " ";
            oss << "L" << (i + 1) << ":" << teamInfo.levelCounts[i];
        }
        sf::Text levelsText;
        levelsText.setFont(font_);
        levelsText.setString(oss.str());
        levelsText.setCharacterSize(9);
        levelsText.setFillColor(sf::Color(180, 180, 180, 255));
        levelsText.setPosition(area_.left + padding_ + 20.0f, y);
        target.draw(levelsText);
        y += lineHeight_;
        if (teamInfo.victoryProgress > 0.0f) {
            sf::RectangleShape progressBg;
            progressBg.setSize(sf::Vector2f(100.0f, 8.0f));
            progressBg.setPosition(area_.left + padding_ + 20.0f, y);
            progressBg.setFillColor(progressBackColor_);
            target.draw(progressBg);
            sf::RectangleShape progressBar;
            progressBar.setSize(sf::Vector2f(100.0f * std::min(1.0f, teamInfo.victoryProgress), 8.0f));
            progressBar.setPosition(area_.left + padding_ + 20.0f, y);
            progressBar.setFillColor(teamInfo.isWinning ? winningTeamColor_ : sf::Color::Yellow);
            target.draw(progressBar);
        }
        y += sectionSpacing_;
    }
}

}
