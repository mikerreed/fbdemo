#include "include/color.h"
#include "include/data.h"
#include "include/time.h"
#include "include/content.h"

#include "ecma/jsc2d_canvas.h"

#include <emscripten/bind.h>

#include <stdio.h>

//using namespace emscripten;

using namespace pentrek;

void Content::RequestDraw(Content*) {
    ptrk_request_animation_frame();
}

std::unique_ptr<pentrek::Content> MakeScribble();

static int gContentIndex;
static std::unique_ptr<Content> gContent;
static std::unique_ptr<Click> gClick;

static void flush_mouse_up() {
    if (gClick) {
        gClick->up();
        gClick = nullptr;
    }
}

void make_content() {
    flush_mouse_up();

    gContent = Content::Make(gContentIndex);
    auto name = gContent->title();
    printf("make content %d %s\n", gContentIndex, name.c_str());
}

static void inc_content(int inc) {
    gContentIndex += inc;
    while (gContentIndex < 0) {
        gContentIndex += Content::Count();
    }
    gContentIndex = gContentIndex % Content::Count();
    
    make_content();
}

void dispatch_draw(C2DContextID ctx) {
    if (gContent) {
        gContent->setAbsTime(GlobalTime::Secs());

        JSC2DCanvas canvas(ctx);
        gContent->draw(&canvas);
    }
}

enum class MouseEventType {
    down,
    up,
    move,
    hover,
};

void dispatch_mouse_event(float x, float y, int etype) {
    if (gContent) {
        switch ((MouseEventType)etype) {
            case MouseEventType::down:
                flush_mouse_up();
                gClick = gContent->findClick({x, y});
                break;
            case MouseEventType::up:
                flush_mouse_up();
                break;
            case MouseEventType::move:
                if (gClick) {
                    gClick->moved({x, y});
                }
                break;
            case MouseEventType::hover:
                flush_mouse_up();
                gContent->handleHover({x, y});
                break;
            default:
                printf("UNEXPECTED mouse-event-type %d\n", etype);
                break;
        }
    }
}

static KeyCode str_to_code(const std::string& str) {
    const std::pair<const char*, KeyCode> table[] = {
        {"ArrowUp",    KeyCode::arrow_up},
        {"ArrowDown",  KeyCode::arrow_down},
        {"ArrowLeft",  KeyCode::arrow_left},
        {"ArrowRight", KeyCode::arrow_right},

        {"PageUp",   KeyCode::page_up},
        {"PageDown", KeyCode::page_down},

        {"NumpadEnter", KeyCode::enter_code},
        {"Enter",       KeyCode::return_code},
        {"Escape",      KeyCode::escape_code},
        {"Delete",      KeyCode::delete_code},  // forward_delete?
        {"Backspace",   KeyCode::delete_code},  // backspace?
    };
    
    for (auto p : table) {
        if (str == p.first) {
            return p.second;
        }
    }
    return KeyCode::none;
}

bool dispatch_key_down(std::string codeStr, Unichar uni,
                       bool shift, bool ctrl, bool opt, bool cmd) {
    if (!gContent) {
        return false;
    }

    unsigned modi = 0;
    if (shift) modi |= KeyModifiers::shift;
    if (ctrl ) modi |= KeyModifiers::control;
    if (opt  ) modi |= KeyModifiers::option;
    if (cmd  ) modi |= KeyModifiers::command;

    KeyCode code = str_to_code(codeStr);

    if (code != KeyCode::none) {
        uni = 0;
    }

    if (gContent->keyDown({uni, uni, code, modi})) {
        return true;
    }
    
    switch (code) {
        case KeyCode::arrow_left:  inc_content(-1); return true;
        case KeyCode::arrow_right: inc_content(+1); return true;
        default:
            break;
    }
    return false;
}


EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("make_content", &make_content);

    emscripten::function("dispatch_draw", &dispatch_draw);
    emscripten::function("dispatch_mouse_event", &dispatch_mouse_event);
    emscripten::function("dispatch_key_down", &dispatch_key_down);
}
