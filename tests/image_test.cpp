/* SPDX-FileCopyrightText: Copyright © 2024 Dominik Wójt <domin144@o2.pl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <image.hpp>
#include <SampleImages.hpp>

#include <gtest/gtest.h>

class TemporaryDirectory
{
private:
    std::filesystem::path m_path;

public:
    TemporaryDirectory() : m_path{"temporary_directory"}
    {
        std::filesystem::create_directory(m_path);
    }

    ~TemporaryDirectory()
    {
        // std::filesystem::remove_all(m_path);
    }

    const std::filesystem::path &getPath() const
    {
        return m_path;
    }
};

TEST(write_png_file_exists, io)
{
    TemporaryDirectory temporaryDirectory;
    Image image = makeButterFly();
    const std::filesystem::path butterflyPath = temporaryDirectory.getPath() / "butterfly.png";
    image.save(butterflyPath);
    EXPECT_TRUE(std::filesystem::exists(butterflyPath));
}

