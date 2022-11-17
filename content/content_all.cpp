/*
 *  Copyright Pentrek Inc, 2022
 */

#include "include/content.h"

using namespace pentrek;

std::unique_ptr<Content> MakeScribble();
std::unique_ptr<Content> MakeFollowPath();
std::unique_ptr<Content> MakeTrimContent();
std::unique_ptr<Content> MakeDashContent();
std::unique_ptr<Content> MakeTextContent();
std::unique_ptr<Content> MakeCatmullPath();
std::unique_ptr<Content> MakePatchContent();
std::unique_ptr<Content> MakeEnvelopeContent();
std::unique_ptr<Content> MakeCubicUnitContent();
std::unique_ptr<Content> MakeInnerOuterContent();
std::unique_ptr<Content> MakeKeyFramesContent();
std::unique_ptr<Content> MakeTextlineContent();
std::unique_ptr<Content> MakeMouseOverContent();
std::unique_ptr<Content> MakeTextOnPathContent();
std::unique_ptr<Content> MakeSnakeContent();

using ContentMaker = std::unique_ptr<pentrek::Content> (*)();

const ContentMaker makers[] = {
    MakeScribble,
#ifdef PENTREK_BUILD_FOR_APPLE
    MakeFollowPath,
    MakeTrimContent,
    MakeDashContent,
    MakeCatmullPath,
    MakePatchContent,
    MakeCubicUnitContent,
    MakeInnerOuterContent,

    MakeTextContent,
    MakeEnvelopeContent,
    MakeKeyFramesContent,
    MakeTextlineContent,
    MakeMouseOverContent,
    MakeTextOnPathContent,

    MakeSnakeContent,
#endif
};

int Content::Count() { return ArrayCount(makers); }
std::unique_ptr<Content> Content::Make(int index) {
    assert((unsigned)index < ArrayCount(makers));
    return makers[index]();
}
