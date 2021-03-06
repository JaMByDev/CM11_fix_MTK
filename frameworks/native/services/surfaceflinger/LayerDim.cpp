/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/Log.h>

#include <ui/GraphicBuffer.h>

#include "LayerDim.h"
#include "SurfaceFlinger.h"
#include "DisplayDevice.h"
#include "RenderEngine/RenderEngine.h"

#ifdef MTK_MT6577
#include <hardware/hwcomposer.h>
#endif

namespace android {
// ---------------------------------------------------------------------------

LayerDim::LayerDim(SurfaceFlinger* flinger, const sp<Client>& client,
        const String8& name, uint32_t w, uint32_t h, uint32_t flags)
    : Layer(flinger, client, name, w, h, flags) {
}

LayerDim::~LayerDim() {
}

void LayerDim::onDraw(const sp<const DisplayDevice>& hw, const Region& clip) const
{
    const State& s(getDrawingState());
    if (s.alpha>0) {
        Mesh mesh(Mesh::TRIANGLE_FAN, 4, 2);
        computeGeometry(hw, mesh);
        RenderEngine& engine(mFlinger->getRenderEngine());
        engine.setupDimLayerBlending(s.alpha);
        engine.drawMesh(mesh);
        engine.disableBlending();
    }
}

bool LayerDim::isVisible() const {
    const Layer::State& s(getDrawingState());
    return !(s.flags & layer_state_t::eLayerHidden) && s.alpha;
}

#ifdef MTK_MT6577
void LayerDim::setGeometry(
    const sp<const DisplayDevice>& hw,
        HWComposer::HWCLayerInterface& layer)
{
    Layer::setGeometry(hw, layer);

    layer.setLayerType(HWC_DIM_LAYER);

    const State& s(getDrawingState());
    hwc_color_t color;
    color.a = s.alpha;
    layer.setFillColor(color);
    if (s.alpha != 0xFF) {
        layer.setBlending(HWC_BLENDING_PREMULT);
    }
    
    const Transform tr(hw->getTransform() * s.transform);
    layer.setTransform(tr.getOrientation());
    layer.setMatrix(tr);
}

void LayerDim::setPerFrameData(const sp<const DisplayDevice>& hw,
        HWComposer::HWCLayerInterface& layer) 
{
    Layer::setPerFrameData(hw, layer);
    layer.setBuffer(NULL);
}

#endif
// ---------------------------------------------------------------------------

}; // namespace android
