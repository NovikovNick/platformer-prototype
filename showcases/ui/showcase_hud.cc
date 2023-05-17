#include "showcase_hud.h"

#include <imgui-SFML.h>
#include <imgui.h>

namespace platformer {

platformer::ShowcaseHUD::ShowcaseHUD(sf::RenderWindow& window)
    : selected_player_id_(0), player_select_labels_({"1st player", "2nd player"}) {
  ImGui::SFML::Init(window);
}

ShowcaseHUD::~ShowcaseHUD() { ImGui::SFML::Shutdown(); }

void ShowcaseHUD::handleEvent(sf::RenderWindow& window, sf::Event& event) const {
  ImGui::SFML::ProcessEvent(window, event);
}

void ShowcaseHUD::draw(sf::RenderWindow& window,
                       ShowcaseContext& ctx,
                       ShowcaseCallback& cb) {
  ImGui::SFML::Update(window, deltaClock.restart());
  ImGui::SetNextWindowPos({350, 0});
  ImGui::SetNextWindowContentSize({530, 200});
  ImGui::Begin("Settings");
  ImGui::InputText(
      "Your public ip:port", ctx.local_public_ip, 22, ImGuiInputTextFlags_ReadOnly);
  ImGui::InputText("Peer public ip:port", ctx.remote_public_ip, 22);

  if (ImGui::BeginCombo("Tickrate", std::to_string(ctx.tick_rate).c_str())) {
    for (int i = 10; i <= 60; i += 10) {
      bool isSelected = ctx.tick_rate == i;
      if (ImGui::Selectable(std::to_string(i).c_str(), isSelected))
        ctx.tick_rate = i;
      if (isSelected) ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  if (ImGui::BeginCombo("Player",
                        player_select_labels_[ctx.active_player_id].c_str())) {
    for (int i = 0; i < player_select_labels_.size(); ++i) {
      bool isSelected = (ctx.active_player_id == i);
      if (ImGui::Selectable(player_select_labels_[i].c_str(), isSelected))
        ctx.active_player_id = i;
      if (isSelected) ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  ImGui::Checkbox(std::string("Log delta time").c_str(), &ctx.log_dx);
  ImGui::Checkbox(std::string("Log player state").c_str(), &ctx.log_player_state);

  if (ImGui::Button("Start session")) cb.on_restart_handler();

  ImGui::End();

  ImGui::SFML::Render(window);
};
}  // namespace platformer