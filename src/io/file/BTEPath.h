#pragma once

/*
    FilePath class ended up being a disaster because of repeated conversions between string paths and FilePaths.
    Also, the usage of getModifiedPath() and getUnmodifiedPath() within FileHandler is extremely messy.
    Unfortunately, most of FileHandler relies on FilePath.

    The plan is to slowly replace FilePath with BTEPath.
    BTEPath will store the full filesystem path string and take in a single string in its constructor (regardless of format: "/home/user/.../backtoearth" or "/backtoearth/" or whatever).

    However, this may not be implemented for a while as the current system "just works".
*/

class BTEPath {

};