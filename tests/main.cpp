/* SPDX-FileCopyrightText: Copyright © 2024 Dominik Wójt <domin144@o2.pl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <gtest/gtest.h>
#include <FreeImagePlus.h>

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

int main(int argc, char **argv)
{
    FreeImage_Initialise();
    FreeImage_SetOutputMessage(FreeImageErrorHandler);

    testing::InitGoogleTest(&argc, argv);
    const int result = RUN_ALL_TESTS();

    FreeImage_DeInitialise();
    return result;
}