#ifndef BZ_ANIMATION_H
#define BZ_ANIMATION_H

#include <memory>
#include <stdint.h>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "display/palette.h"

#include "data.h"

/**
 * This class is an interface for an animation loaded from a .abz
 * animation file.
 *
 * TODO: Add duration methods to show the animation length & time
 * remaining.
 */
class BZAnimation
{
public:
    typedef boost::shared_ptr<BZAnimation> Ptr;

    static Ptr load(const char* file, const char* id, int x, int y);

    ~BZAnimation();

    void advance();

private:
    int mX, mY;
    int mCurrentFrame;
    // mDisplay is where frames are staged before being drawn.
    std::unique_ptr<display::LegacySurface> mDisplay;
    AnimType mHeader;
    std::vector<std::unique_ptr<uint8_t[]>> mFrameData;

    BZAnimation(AnimType header, display::Palette palette,
                std::vector<uint8_t*> frames, int x, int y);
};

#endif // BZ_ANIMATION_H
