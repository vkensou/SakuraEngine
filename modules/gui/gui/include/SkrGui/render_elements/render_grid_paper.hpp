#pragma once
#include "SkrGui/framework/render_element.hpp"

SKR_DECLARE_TYPE_ID_FWD(skr::gdi, GDIDevice, skr_gdi_device)
SKR_DECLARE_TYPE_ID_FWD(skr::gdi, GDIElement, skr_gdi_element)

namespace skr {
namespace gui {

struct SKR_GUI_API RenderGridPaper : public RenderElement
{
public:
    RenderGridPaper(skr_gdi_device_id gdi_device);
    virtual ~RenderGridPaper();

    virtual void layout(struct Constraints* constraints, bool needSize = false) override;
    virtual void draw(const DrawParams* params) override;

    virtual skr_float2_t get_size() const;
    virtual void set_size(const skr_float2_t& size);

    skr_gdi_device_id gdi_device = nullptr;
    skr_gdi_element_id gdi_element = nullptr;
};

} }

SKR_DECLARE_TYPE_ID(skr::gui::RenderGridPaper, skr_gui_render_grid_paper);