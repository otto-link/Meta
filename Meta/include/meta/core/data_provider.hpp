/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "meta/type/attribute_traits.hpp"
#include "meta/type/type_name.hpp"

namespace meta
{

/// Qt-free payload a host supplies to a widget for runtime-computed display data.
struct ProviderData
{
  std::vector<float>   series_x;             ///< e.g. histogram bin centers
  std::vector<float>   series_y;             ///< e.g. histogram counts/heights
  int                  image_width    = 0;
  int                  image_height   = 0;
  int                  image_channels = 0;   ///< 1, 3, or 4
  std::vector<uint8_t> image_pixels;         ///< row-major, channel-interleaved

  bool has_series() const { return !series_y.empty(); }
  bool has_image() const
  {
    return image_width > 0 && image_height > 0 && !image_pixels.empty();
  }
};

/// Host-supplied callback returning fresh display data on each call. Non-serializable.
using DataProvider = std::function<ProviderData()>;

/// No-op traits: a DataProvider carries runtime state that must not be serialized.
template <> struct AttributeTraits<DataProvider>
{
  static std::string   to_string(const DataProvider &) { return "<data_provider>"; }
  static nlohmann::json json_to(const DataProvider &) { return nullptr; }
  static DataProvider   json_from(const nlohmann::json &) { return {}; }
};

} // namespace meta

META_DEFINE_TYPE_NAME(meta::DataProvider);
