/*
 *  Copyright Pentrek Inc, 2022
 */

#ifndef _pentrek_canvas_h_
#define _pentrek_canvas_h_

#include "include/color.h"
#include "include/matrix.h"
#include "include/path.h"
#include "include/shader.h"

namespace pentrek {

class Paint {
    rcp<Shader> m_shader;
    Color fColor{0, 0, 0, 1};
    float fWidth = 1;
    uint32_t fFlags = 0;

    enum Flags {
        kStroke = 1 << 0,
    };

public:
    Paint() = default;
    Paint(const Paint&) = default;
    Paint(const Color& c) : fColor(c) {}

    bool isStroke() const { return (fFlags & kStroke) != 0; }
    bool isFill() const { return !this->isStroke(); }
    
    Color color() const { return fColor; }
    Color32 color32() const { return fColor.color32(); }
    float width() const { return fWidth; }

    void stroke(bool isStroke) {
        if (isStroke) {
            fFlags |= kStroke;
        } else {
            fFlags &= ~kStroke;
        }
    }
    void fill(bool isFill) { this->stroke(!isFill); }

    void color(const Color& c) { fColor = c; }
    void color(float r, float g, float b, float a = 1) {
        this->color({r, g, b, a});
    }
    void width(float w) { assert(w > 0); fWidth = w; }
    
    Shader* shader() const { return m_shader.get(); }
    rcp<Shader> refShader() const { return m_shader; }
    void shader(rcp<Shader> sh) { m_shader = sh; }
};

class Canvas {
public:
    virtual ~Canvas() {}
    
    void save();
    void restore();
    
    int saveCount() const { return m_saveCount; }
    void restoreToCount(int);

    void translate(Point);
    void scale(Point);
    void rotate(float radians);
    void concat(const Matrix&);

    void clipRect(const Rect&);
    void clipPath(const Path&);
    void clipPath(const rcp<const Path>& path) { this->clipPath(*path.get()); }

    void drawRect(const Rect&, const Paint&);
    void drawOval(const Rect&, const Paint&);
    void drawCircle(Point center, float radius, const Paint&);
    void drawPoly(Span<const Point>, bool doClose, const Paint&);
    void drawPath(const Path&, const Paint&);
    void drawPath(const rcp<const Path>& path, const Paint& paint) {
        this->drawPath(*path.get(), paint);
    }

    void drawPoints(Span<const Point>, const Paint&);
    void drawPoint(Point, const Paint&);
    void drawLine(Point, Point, const Paint&);

    class AutoRestore {
        Canvas* m_canvas;
        const int m_prevSaveCount;
    public:
        AutoRestore(Canvas* canvas, bool doSave = true)
            : m_canvas(canvas)
            , m_prevSaveCount(canvas->saveCount())
        {
            if (doSave) {
                canvas->save();
            }
        }
        ~AutoRestore() {
            m_canvas->restoreToCount(m_prevSaveCount);
        }
    };

    void translate(float x, float y) { this->translate({x, y}); }
    void scale(float x, float y) { this->scale({x, y}); }

protected:
    virtual void onSave() = 0;
    virtual void onRestore() = 0;

    virtual void onConcat(const Matrix&) = 0;    
    virtual void onClipRect(const Rect&);
    virtual void onClipPath(const Path&) = 0;
    
    virtual void onDrawRect(const Rect&, const Paint&);
    virtual void onDrawPath(const Path&, const Paint&) = 0;

private:
    int m_saveCount = 0;
};

}
#endif
