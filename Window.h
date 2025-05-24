#ifndef WINDOW_H
#define WINDOW_H

#include "IWindow.h"
#include "ErrorMessanger.h"
#include "FileReader.h"
#include "Parser.h"
#include "Config.h"
#include "View.h"
#include "Stage.h"

#include <QMainWindow>
#include <memory>
#include <thread>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Window;
}
QT_END_NAMESPACE

class Window : public QMainWindow, public Core::IWindow, public Core::ISafe
{
    Q_OBJECT
public:
    Window(QWidget* parent = nullptr);
    virtual ~Window();
    virtual void add_sensor_line(const Core::x_string_t& line) override final;
    virtual void clear_sensor_lines(void) override final;
    virtual void add_error_line(const Core::x_string_t& line) override final;
    virtual void clear_error_lines(void) override final;
    virtual void add_sensor_name(const Core::x_string_t& name) override final;
    virtual void clear_sensor_names(void) override final;
    virtual void set_sensor_title(const Core::x_string_t& title) override final;
    virtual void set_sensor_chasis_formula(const Core::x_string_t& formula) override final;
    virtual void add_sensor_info_line(const Core::x_string_t& line) override final;
    virtual void set_sensor_average_velocity(const Core::x_string_t& average_velocity) override final;
    virtual void set_sensor_velocity_deviation(const Core::x_string_t& velocity_deviation) override final;
    virtual void clear_sensor_info(void) override final;
    virtual void set_overall_chasis_formula(const Core::x_string_t& formula) override final;
    virtual void set_overall_length_formula(const Core::x_string_t& formula) override final;
    virtual void clear_overall_info(void) override final;

private slots:
    void on_actionOpen_triggered();
    void on_sensorPickList_itemSelectionChanged();
    void on_timeRelativeRadioButton_clicked();
    void on_timeAbsoluteRadioButton_clicked();
    void on_playButton_clicked();
    void on_stopButton_clicked();

private:
    void prepare(void);
    void clear_stage(void);
    void set_can_play(const bool can_play);
    void set_is_playing(const bool is_playing);
    void ui_update_play(void);
    void animation(void);

    Ui::Window* ui;
    std::unique_ptr<Core::IErrorMessanger> m_error_messanger;
    std::unique_ptr<Core::IFileReader> m_file_reader;
    std::unique_ptr<Storage::IParser> m_parser;
    std::unique_ptr<Storage::IConfig> m_config;
    std::unique_ptr<Core::View> m_view;
    std::unique_ptr<Model::Stage> m_stage;
    bool m_can_play;
    bool m_is_playing;
    std::unique_ptr<std::thread> m_animator;
};

#endif
