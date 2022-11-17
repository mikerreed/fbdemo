/*
 *  Copyright Pentrek Inc, 2022
 */

#include "include/cg_canvas.h"
#include "include/path.h"

using namespace pentrek;

static CGAffineTransform toCG(const Matrix& m) {
    return CGAffineTransformMake(m[0], m[1], m[2], m[3], m[4], m[5]);
}

static CGPoint toCG(pentrek::Point p) {
    return CGPoint{p.x, p.y};
}

void set_path(CGContextRef ctx, const Path& path) {
    using P = pentrek::Point;

    CGContextBeginPath(ctx);
    path.visit([ctx](const P* p) { CGContextMoveToPoint(ctx, p[0].x, p[0].y); },
               [ctx](const P* p) { CGContextAddLineToPoint(ctx, p[0].x, p[0].y); },
               [ctx](const P* p) { CGContextAddQuadCurveToPoint(ctx, p[0].x, p[0].y, p[1].x, p[1].y); },
               [ctx](const P* p) { CGContextAddCurveToPoint(ctx, p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y); },
               [ctx](P, P) { CGContextClosePath(ctx); });
}

static void apply_stroke(CGContextRef ctx, const Paint& paint) {
    if (paint.isStroke()) {
//        CGContextSetLineCap(ctx, kCGLineCapButt);
//        CGContextSetLineJoin(ctx, kCGLineJoinMiter);
        CGContextSetLineWidth(ctx, paint.width());
    }
}

static void apply(CGContextRef ctx, const Paint& paint) {
    apply_stroke(ctx, paint);

    auto c = paint.color();
    if (auto sh = paint.shader()) {
        assert(sh->type() == Shader::Type::kColor);
        sh->asColor(&c);
    }

    if (paint.isFill()) {
        CGContextSetRGBFillColor(ctx, c.r, c.g, c.b, c.a);
    } else {
        CGContextSetRGBStrokeColor(ctx, c.r, c.g, c.b, c.a);
    }
}

static void draw_gradient(CGContextRef ctx, Shader* sh) {
    assert(sh);
    
    Shader::GradientInfo ginfo;
    DEBUG_CODE(bool success =) sh->asGradient(&ginfo);
    assert(success);

    const size_t N = ginfo.m_colors.size();
    std::vector<CGFloat> storage(N * 5);    // room for rgba + pos
    size_t i = 0;
    for (const auto& c : ginfo.m_colors) {
        storage[i+0] = c.r;
        storage[i+1] = c.g;
        storage[i+2] = c.b;
        storage[i+3] = c.a;
        i += 4;
    }
    CGFloat* pos = nullptr;
    if (ginfo.m_pos) {
        pos = &storage[i];  // after last color component
        for (i = 0; i < N; ++i) {
            pos[i] = ginfo.m_pos[i];
        }
    }
    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    CGGradientRef gradient = CGGradientCreateWithColorComponents(cs, storage.data(), pos, N);
    CGColorSpaceRelease(cs);

    auto clamp = kCGGradientDrawsBeforeStartLocation | kCGGradientDrawsAfterEndLocation;

    switch (sh->type()) {
        case Shader::Type::kLinearGradient: {
            Shader::LinearGradientInfo linfo;
            sh->asLinearGradient(&linfo);
            CGContextDrawLinearGradient(ctx, gradient,
                                        toCG(linfo.m_points[0]), toCG(linfo.m_points[1]),
                                        clamp);
        } break;
        case Shader::Type::kRadialGradient: {
            Shader::RadialGradientInfo rinfo;
            sh->asRadialGradient(&rinfo);
            CGContextDrawRadialGradient(ctx, gradient,
                                        toCG(rinfo.m_center), 0,
                                        toCG(rinfo.m_center), rinfo.m_radius,
                                        clamp);
        } break;
        default: assert(false); break;
    }
    CGGradientRelease(gradient);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

CGCanvas::CGCanvas(CGContextRef ctx, float height) : fCtx(ctx) {
    CGContextSaveGState(ctx);
    CGContextConcatCTM(ctx, CGAffineTransformMake(1, 0, 0, -1, 0, height));
}

CGCanvas::~CGCanvas() {
    CGContextRestoreGState(fCtx);
}

void CGCanvas::onSave() { CGContextSaveGState(fCtx); }
void CGCanvas::onRestore() { CGContextRestoreGState(fCtx); }
void CGCanvas::onConcat(const Matrix& m) {
    CGContextConcatCTM(fCtx, toCG(m));
}

void CGCanvas::onClipRect(const Rect& r) {
    CGContextClipToRect(fCtx, CGRectMake(r.x(), r.y(), r.width(), r.height()));
}

void CGCanvas::onClipPath(const Path& path) {
    set_path(fCtx, path);
    CGContextClip(fCtx);
}

static bool has_gradient(const Paint& paint) {
    auto sh = paint.shader();
    return sh && sh->type() != Shader::Type::kColor;
}

void CGCanvas::onDrawRect(const Rect& r, const Paint& paint) {
    if (has_gradient(paint) && paint.isStroke()) {
        this->drawPath(Path::Rect(r), paint);
        return;
    }

    const auto cgr = CGRectMake(r.x(), r.y(), r.width(), r.height());

    if (has_gradient(paint)) {
        AutoRestore ar(this, true);

        CGContextClipToRect(fCtx, cgr);
        draw_gradient(fCtx, paint.shader());
    } else {
        apply(fCtx, paint);
        
        if (paint.isStroke()) {
            CGContextStrokeRect(fCtx, cgr);
        } else {
            CGContextFillRect(fCtx, cgr);
        }
    }
}

void CGCanvas::onDrawPath(const Path& path, const Paint& paint) {
    if (path.empty()) {
        return;
    }

    set_path(fCtx, path);

    if (has_gradient(paint)) {
        AutoRestore ar(this, true);

        if (paint.isStroke()) {
            apply_stroke(fCtx, paint);
            CGContextReplacePathWithStrokedPath(fCtx);
        }
        CGContextClip(fCtx);
        draw_gradient(fCtx, paint.shader());
    } else {
        apply(fCtx, paint);
        
        CGPathDrawingMode mode = kCGPathStroke;
        if (paint.isFill()) {
            mode = kCGPathFill; // kCGPathEOFill ?
        }
        CGContextDrawPath(fCtx, mode);
    }
    assert(CGContextIsPathEmpty(fCtx));
}
