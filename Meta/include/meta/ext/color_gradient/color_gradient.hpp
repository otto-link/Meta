/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <array>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace meta
{

/**
 * @brief A color stop in a gradient.
 */
struct Stop
{
  /// Position in the range [0, 1].
  float position;

  /// RGBA color.
  std::array<float, 4> color;
};

/**
 * @brief A named color gradient preset.
 */
struct Preset
{
  /// Preset name.
  std::string name;

  /// Gradient stops.
  std::vector<Stop> stops;
};

/**
 * @brief Editable color gradient with optional presets.
 */
class ColorGradient
{
public:
  /**
   * @brief Constructs a default black-to-white gradient.
   */
  ColorGradient() = default;

  /**
   * @brief Deserializes the object from a JSON representation.
   *
   * @param json Input JSON data used to restore the object state.
   */
  void json_from(nlohmann::json const &json);

  /**
   * @brief Serializes the object to a JSON representation.
   *
   * @return nlohmann::json JSON object representing the current state.
   */
  nlohmann::json json_to() const;

  /**
   * @brief Returns the available presets.
   */
  const std::vector<Preset> &presets() const;

  /**
   * @brief Sets the gradient stops.
   */
  void set_value(const std::vector<Stop> &new_value);

  /**
   * @brief Sets the available presets.
   */
  void set_presets(const std::vector<Preset> &new_presets);

  /**
   * @brief Returns the gradient stops.
   */
  const std::vector<Stop> &value() const;

  /**
   * @brief Returns the gradient stops.
   */
  std::vector<Stop> &value();

private:
  std::vector<Stop>   value_ = {{0.f, {0.f, 0.f, 0.f, 1.f}},
                                {1.f, {1.f, 1.f, 1.f, 1.f}}};
  std::vector<Preset> presets_;
};

} // namespace meta

#include "meta/ext/color_gradient/color_gradient.inl"