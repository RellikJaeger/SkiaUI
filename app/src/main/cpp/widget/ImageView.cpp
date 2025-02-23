//
// Created by cw on 1/26/22.
//

#include <base/native_log.h>
#include "core/SkColorFilter.h"
#include "ImageView.h"
#include "core/SkData.h"

ImageView::ImageView() : View(), radius(0), scaleType(ScaleType::FitXY) {
    imagePaint = std::make_unique<SkPaint>();
    imagePaint->setAntiAlias(true);
    srcRect.setEmpty();
    dstRect.setEmpty();
    clipRect.setEmpty();
    imageMatrix = SkMatrix::I();
}

ImageView::~ImageView() {
//    ALOGD("~View ImageView")
}

void ImageView::setAlpha(float alpha) {
    View::setAlpha(alpha);
    SkASSERT(imagePaint);
    imagePaint->setAlphaf(alpha);
}

void ImageView::setSource(const char *path) {
    auto data = SkData::MakeFromFileName(path);
    skImage = SkImage::MakeFromEncoded(data);
    if (skImage == nullptr) {
        ALOGE("skImage is null, pls check %s", path)
        return;
    }
    srcRect.setWH(static_cast<float>(skImage->width()), static_cast<float >(skImage->height()));
    isDirty = true;
}

void ImageView::measure(int widthMeasureSpec, int heightMeasureSpec) {
    if (skImage == nullptr) {
        setMeasuredDimension(MeasureSpec::getSize(widthMeasureSpec),
                             MeasureSpec::getSize(heightMeasureSpec));
        return;
    }
    if (isDirty) {
        if (MeasureSpec::getMode(widthMeasureSpec) == EXACTLY &&
            MeasureSpec::getMode(heightMeasureSpec) == EXACTLY) {
            auto width = MeasureSpec::getSize(widthMeasureSpec);
            auto height = MeasureSpec::getSize(heightMeasureSpec);
            setMeasuredDimension(width, height);
            ALOGD("imageView size: %d %d", width, height)
            return;
        }
        ALOGD("imageView size: %d %d", skImage->width(), skImage->height())
        setMeasuredDimension(skImage->width(), skImage->height());
    }
}

void ImageView::layout(int l, int t, int r, int b) {
    View::layout(l, t, r, b);
    dstRect.setLTRB(static_cast<float >(l), static_cast<float >(t), static_cast<float >(r),
                    static_cast<float >(b));
    if (skImage == nullptr) {
        return;
    }
    imageMatrix.setIdentity();
    if (scaleType == ScaleType::FitCenter) {
        auto imageRatio =
                static_cast<float>(skImage->width()) / static_cast<float >(skImage->height());
        auto viewRatio = dstRect.width() / dstRect.height();
        if (imageRatio > viewRatio) {
            imageMatrix.preScale(1.0f, viewRatio / imageRatio, dstRect.centerX(),
                                 dstRect.centerY());
        } else {
            imageMatrix.preScale(imageRatio / viewRatio, 1.0f, dstRect.centerX(),
                                 dstRect.centerY());
        }
    } else if (scaleType == ScaleType::CenterCrop) {
        auto imageRatio =
                static_cast<float>(skImage->width()) / static_cast<float >(skImage->height());
        auto viewRatio = dstRect.width() / dstRect.height();
        if (imageRatio > viewRatio) {
            imageMatrix.preScale(imageRatio / viewRatio, 1.0f, dstRect.centerX(),
                                 dstRect.centerY());
        } else {
            imageMatrix.preScale(1.0f, viewRatio / imageRatio, dstRect.centerX(),
                                 dstRect.centerY());
        }
    }
}

void ImageView::draw(SkCanvas *canvas) {
    if (width == 0 || height == 0) {
        ALOGE("ignore ImageView draw, pls check width and height %d %d", width, height)
        return;
    }
    canvas->save();
    clipRect.setRectXY(dstRect, radius, radius);
    canvas->clipRRect(clipRect);
    canvas->setMatrix(imageMatrix);
    canvas->drawImageRect(skImage, srcRect, dstRect, SkSamplingOptions(), imagePaint.get(),
                          SkCanvas::kFast_SrcRectConstraint);
    canvas->restore();
    View::draw(canvas);
}

const char *ImageView::name() {
    return "ImageView";
}

void ImageView::setCornerRadius(int radius) {
    this->radius = static_cast<float >(radius);
    View::setCornerRadius(radius);
    isDirty = true;
}

void ImageView::setScaleType(ImageView::ScaleType scaleType) {
    this->scaleType = scaleType;
    isDirty = true;
}


