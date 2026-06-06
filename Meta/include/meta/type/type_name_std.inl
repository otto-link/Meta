/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file type_name_stl.inl
 * @copyright Copyright (c) 2026
 */
#pragma once
#include <filesystem>
#include <string>
#include <vector>

META_DEFINE_TYPE_NAME(std::filesystem::path);

META_DEFINE_TYPE_NAME(std::vector<float>);
META_DEFINE_TYPE_NAME(std::vector<double>);
META_DEFINE_TYPE_NAME(std::vector<int>);
META_DEFINE_TYPE_NAME(std::vector<bool>);

META_DEFINE_TYPE_NAME(std::vector<std::string>);
