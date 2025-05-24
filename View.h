#ifndef VIEW_H
#define VIEW_H

#include "ISafe.h"
#include "IWindow.h"
#include "IConfig.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

namespace Core
{
    class View : public ISafe
    {
    public:
        View(IWindow& window, IErrorMessanger& error_messanger, Storage::IConfig& config, QMainWindow& holder, QGraphicsView& parent);
        void draw_chasis(const Math::chasis_formula_t& length_formula, const Math::position_t& position_meters);
        void clear_chasis(void);
        void clear(void);

    private:
        View(const View&) = delete;
        View(View&&) = delete;
        View& operator=(const View&) = delete;
        View& operator=(View&&) = delete;

        void prepare(void);
        inline canpos_t vp2cp_x(const canpos_t& value_pixels) const;
        inline canpos_t vp2cp_y(const canpos_t& value_pixels) const;
        inline canpos_t vr2vp_y(const canrat_t& value_percents) const;
        inline canpos_t vm2vp_x(const Math::position_t& value_meters) const;

        IWindow& m_window;
        IErrorMessanger& m_error_messanger;
        Storage::IConfig& m_config;
        QMainWindow& m_holder;
        QGraphicsView& m_parent;
        const Math::position_t m_track_length_meters;
        const canpos_t m_canvas_width_pixels;
        const canpos_t m_canvas_height_pixels;
        const canpos_t m_view_width_pixels;
        const canpos_t m_view_height_pixels;
        const canpos_t m_view_stem_y_offset_pixels;
        QPen m_view_stem_pen;
        const canpos_t m_view_sensor_y_offset_pixels;
        QPen m_view_sensor_pen;
        QPen m_view_wheel_pen;
        QBrush m_view_wheel_brush;
        std::unique_ptr<QGraphicsScene> m_scene;
        const Math::chasis_formula_t* m_length_formula;
        std::vector<QGraphicsEllipseItem*> m_chasis;


        static constexpr const canpos_t s_horizontal_margin_pixels = 8.0;
        static constexpr const canpos_t s_vertical_margin_pixels = 8.0;
        static constexpr const canrat_t s_view_stem_y_offset_percents = 0.2;
        static constexpr const canpos_t s_view_stem_pen_width_pixels = 2.0;
        static constexpr const canrat_t s_view_sensor_y_offset_percents = 0.3;
        static constexpr const canpos_t s_view_sensor_pen_width_pixels = 2.75;
        static constexpr const Math::position_t s_view_wheel_size_meters = 1.05;
        static constexpr const canpos_t s_view_wheel_pen_width_pixels = 1.0;
    };
}

#endif
