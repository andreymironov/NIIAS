#include "View.h"

#include <sstream>

using namespace Math;
using namespace Storage;

namespace Core
{
    View::View(IWindow& window, IErrorMessanger& error_messanger, IConfig& config, QMainWindow& holder, QGraphicsView& parent) :
        m_window(window),
        m_error_messanger(error_messanger),
        m_config(config),
        m_holder(holder),
        m_parent(parent),
        m_track_length_meters(m_config.get_positions().crbegin()->second),
        m_canvas_width_pixels(m_parent.geometry().width()),
        m_canvas_height_pixels(m_parent.geometry().height()),
        m_view_width_pixels(m_canvas_width_pixels - 2.0 * s_horizontal_margin_pixels),
        m_view_height_pixels(m_canvas_height_pixels - 2.0 * s_vertical_margin_pixels),
        m_view_stem_y_offset_pixels(vr2vp_y(s_view_stem_y_offset_percents)),
        m_view_stem_pen(Qt::black),
        m_view_sensor_y_offset_pixels(vr2vp_y(s_view_sensor_y_offset_percents)),
        m_view_sensor_pen(Qt::blue),
        m_view_wheel_pen(Qt::lightGray),
        m_view_wheel_brush(Qt::gray),
        m_scene(new QGraphicsScene(0, 0, m_canvas_width_pixels, m_canvas_height_pixels, &m_holder)),
        m_length_formula(nullptr)
    {
        safe(this, m_error_messanger, &View::prepare);
    }

    void View::draw_chasis(const Math::chasis_formula_t& length_formula, const Math::position_t& position_meters)
    {
        static const canpos_t wheel_diameter_pixels = vm2vp_x(s_view_wheel_size_meters);
        static const canpos_t wheel_radius_pixels = wheel_diameter_pixels / 2.0;

        if (length_formula.empty())
            return;
        const size_t chasis_size = length_formula.size();
        if (m_chasis.empty())
            m_chasis.resize(chasis_size, nullptr);
        const auto canvas_wheel_y_offset_pixels = vp2cp_y(m_view_stem_y_offset_pixels - wheel_diameter_pixels - s_view_stem_pen_width_pixels);
        for (size_t index = 0; index < chasis_size; ++index)
        {
            const auto wheel_position_meters = position_meters - length_formula[index];
            const auto wheel_position_pixels = vm2vp_x(wheel_position_meters) - wheel_radius_pixels;
            const QRectF rect(vp2cp_x(wheel_position_pixels), canvas_wheel_y_offset_pixels, wheel_diameter_pixels, wheel_diameter_pixels);
            if (m_chasis[index] == nullptr)
                m_chasis[index] = m_scene->addEllipse(rect, m_view_wheel_pen, m_view_wheel_brush);
            else
                m_chasis[index]->setRect(rect);
        }
        m_scene->update();
    }

    void View::clear_chasis(void)
    {
        if (m_chasis.empty())
            return;
        const size_t chasis_size = m_chasis.size();
        for (size_t index = 0; index < chasis_size; ++index)
        {
            if (m_chasis[index] != nullptr)
            {
                m_scene->removeItem(m_chasis[index]);
                m_chasis[index] = nullptr;
            }
        }
        m_scene->update();
    }

    void View::clear(void)
    {
        m_scene->clear();
    }

    void View::prepare(void)
    {
        m_view_stem_pen.setWidth(s_view_stem_pen_width_pixels);
        m_view_sensor_pen.setWidth(s_view_sensor_pen_width_pixels);
        m_view_wheel_pen.setWidth(s_view_wheel_pen_width_pixels);
        m_parent.setScene(m_scene.get());
        m_scene->addLine(0, vp2cp_y(m_view_stem_y_offset_pixels), m_canvas_width_pixels, vp2cp_y(m_view_stem_y_offset_pixels), m_view_stem_pen);
        const auto& sensor_positions = m_config.get_positions();
        canpos_t min_name_x_offset_pixels = 0.0;
        for (const auto& sensor_position : sensor_positions)
        {
            const auto distance_meters = sensor_position.second;
            const auto offset_x_pixels = vm2vp_x(distance_meters);
            m_scene->addLine(vp2cp_x(offset_x_pixels), vp2cp_y(m_view_stem_y_offset_pixels), vp2cp_x(offset_x_pixels), vp2cp_y(m_view_sensor_y_offset_pixels), m_view_sensor_pen);
            {
                std::stringstream stream;
                stream << "#" << static_cast<int>(sensor_position.first);
                auto name = m_scene->addText(QString::fromStdString(stream.str()));
                const canpos_t name_width_pixels = name->boundingRect().width();
                min_name_x_offset_pixels = std::max(min_name_x_offset_pixels, vp2cp_x(offset_x_pixels) - name_width_pixels / 2.0);
                name->setPos(min_name_x_offset_pixels, vp2cp_y(m_view_sensor_y_offset_pixels));
                min_name_x_offset_pixels += name_width_pixels;
            }
        }
    }

    canpos_t View::vp2cp_x(const canpos_t& value_pixels) const
    {
        return value_pixels + s_horizontal_margin_pixels;
    }

    canpos_t View::vp2cp_y(const canpos_t& value_pixels) const
    {
        return value_pixels + s_vertical_margin_pixels;
    }

    canpos_t View::vr2vp_y(const canrat_t& value_percents) const
    {
        return static_cast<canpos_t>(value_percents * static_cast<canrat_t>(m_view_height_pixels));
    }

    canpos_t View::vm2vp_x(const position_t& value_meters) const
    {
        return static_cast<canpos_t>(static_cast<canrat_t>(m_view_width_pixels) * static_cast<canrat_t>(value_meters) / static_cast<canrat_t>(m_track_length_meters));
    }
}
