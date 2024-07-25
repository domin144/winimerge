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

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
    std::cout << "\n*** ";
    if (fif != FIF_UNKNOWN)
    {
        std::cout << FreeImage_GetFormatFromFIF(fif) << " Format\n";
    }
    std::cout << message;
    std::cout << " ***" << std::endl;
}

TEST(test_2_plus_2, test_0)
{
    FreeImage_Initialise();
    FreeImage_SetOutputMessage(FreeImageErrorHandler);

    TemporaryDirectory temporaryDirectory;
    Image image = makeButterFly();
    const std::filesystem::path butterflyPath = temporaryDirectory.getPath() / "butterfly.png";
    image.save(butterflyPath);
    EXPECT_TRUE(std::filesystem::exists(butterflyPath));
    FreeImage_DeInitialise();
}
