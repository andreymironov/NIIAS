#include "Window.h"
#include "ui_Window.h"

#include <QFileDialog>
#include <mutex>
#include <chrono>

Window::Window(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::Window),
    m_error_messanger(new Core::ErrorMessanger(*this)),
    m_file_reader(new Core::FileReader(*m_error_messanger)),
    m_parser(new Storage::Parser()),
    m_config(new Storage::Config(*m_error_messanger, *m_file_reader, *m_parser, Storage::s_config_filename)),
    m_view(nullptr),
    m_stage(nullptr),
    m_can_play(false),
    m_is_playing(false),
    m_animator(nullptr)
{
    ui->setupUi(this);
    safe(this, *m_error_messanger, &Window::prepare);
}

Window::~Window()
{
    clear_stage();
    if (m_view)
    {
        m_view->clear();
        ui->graphicsBackView->setScene(nullptr);
    }
    m_view.reset();
    m_config.reset();
    m_parser.reset();
    m_file_reader.reset();
    m_error_messanger.reset();
    delete ui;
}

void Window::add_sensor_line(const Core::x_string_t& line)
{
    ui->sensorLines->addItem(QString::fromStdWString(line));
}

void Window::clear_sensor_lines(void)
{
    ui->sensorLines->clear();
}

void Window::add_error_line(const Core::x_string_t& line)
{
    ui->errorLines->addItem(QString::fromStdWString(line));
}

void Window::clear_error_lines(void)
{
    ui->errorLines->clear();
}

void Window::add_sensor_name(const Core::x_string_t& name)
{
    ui->sensorPickList->addItem(QString::fromStdWString(name));
}

void Window::clear_sensor_names(void)
{
    ui->sensorPickList->clear();
}

void Window::set_sensor_title(const Core::x_string_t& title)
{
    ui->sensorTitleLabel->setText(QString::fromStdWString(title));
}

void Window::set_sensor_chasis_formula(const Core::x_string_t& formula)
{
    ui->sensorChasisFormulaLabel->setText(QString::fromStdWString(formula));
}

void Window::add_sensor_info_line(const Core::x_string_t& line)
{
    ui->sensorInfoLines->addItem(QString::fromStdWString(line));
}

void Window::set_sensor_average_velocity(const Core::x_string_t& average_velocity)
{
    ui->sensorAverageVelocityLabel->setText(QString::fromStdWString(average_velocity));
}

void Window::set_sensor_velocity_deviation(const Core::x_string_t& velocity_deviation)
{
    ui->sensorVelocityDeviationLabel->setText(QString::fromStdWString(velocity_deviation));
}

void Window::clear_sensor_info(void)
{
    set_sensor_title(L"No sensor info");
    set_sensor_chasis_formula(L"No chasis formula");
    set_sensor_average_velocity(L"No average velocity");
    set_sensor_velocity_deviation(L"No velocity deviation");
    ui->sensorInfoLines->clear();
}

void Window::set_overall_chasis_formula(const Core::x_string_t& formula)
{
    ui->overallChasisFormulaLabel->setText(QString::fromStdWString(formula));
}

void Window::set_overall_length_formula(const Core::x_string_t& formula)
{
    ui->overallLengthFormulaLabel->setText(QString::fromStdWString(formula));
}

void Window::clear_overall_info(void)
{
    set_overall_chasis_formula(L"No chasis formula");
    set_overall_length_formula(L"No length formula");
}

void Window::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Sensor data file"), QDir::currentPath(), tr("Sensor data (*.txt)"));
    if (!filename.isEmpty())
    {
        clear_stage();
        ui->tabWidget->setEnabled(true);
        ui->timeRelativeRadioButton->setChecked(true);
        m_stage.reset(new Model::Stage(*this, *m_error_messanger, *m_file_reader, *m_parser, filename.toStdWString(), *m_config));
        set_can_play(true);
        ui->sensorPickList->setCurrentRow(0);
    }
}

void Window::on_sensorPickList_itemSelectionChanged()
{
    const auto selected_items = ui->sensorPickList->selectedItems();
    if (!selected_items.empty() && m_stage)
    {
        const auto index = static_cast<size_t>(ui->sensorPickList->row(selected_items.front()));
        m_stage->on_sensor_selection(index);
    }
}

void Window::on_timeRelativeRadioButton_clicked()
{
    if (m_stage)
        m_stage->on_time_selection(true);
}

void Window::on_timeAbsoluteRadioButton_clicked()
{
    if (m_stage)
        m_stage->on_time_selection(false);
}

void Window::on_playButton_clicked()
{
    if (!m_can_play || m_is_playing)
        return;
    set_is_playing(true);
}

void Window::on_stopButton_clicked()
{
    if (!m_can_play || !m_is_playing)
        return;
    set_is_playing(false);
}

void Window::prepare(void)
{
    ui->tabWidget->setEnabled(false);
    m_view.reset(new Core::View(*static_cast<Core::IWindow*>(this), *m_error_messanger, *m_config, *static_cast<QMainWindow*>(this), *ui->graphicsBackView));
    const size_t sensors_count = m_config->get_positions().size();
    for (size_t index = 0; index < sensors_count; ++index)
        ui->counterList->addItem("");
}

void Window::clear_stage(void)
{
    if (m_stage)
    {
        set_is_playing(false);
        set_can_play(false);
        m_stage.reset();
    }
}

void Window::set_can_play(const bool can_play)
{
    if (m_can_play == can_play)
        return;
    m_can_play = can_play;
    ui_update_play();
}

void Window::set_is_playing(const bool is_playing)
{
    if (m_is_playing == is_playing)
        return;
    m_is_playing = is_playing;
    ui_update_play();
    if (m_is_playing)
        m_animator.reset(new std::thread(&Window::animation, this));
    else
    {
        if (m_animator)
        {
            if (m_animator->joinable())
            {
                animation();
                m_animator->join();
            }
            m_animator.reset();
        }
        ui->viewStats->clear();
        m_view->clear_chasis();
    }
}

void Window::ui_update_play(void)
{
    ui->playButton->setEnabled(m_can_play && !m_is_playing);
    ui->stopButton->setEnabled(m_can_play && m_is_playing);
}

void Window::animation(void)
{
    static std::mutex mutex;
    static bool is_playing = false;
    {
        const std::lock_guard<std::mutex> guard(mutex);
        is_playing = !is_playing;
        if (!is_playing)
            return;
    }

    static const auto sleep = std::chrono::milliseconds(66);
    const auto start = std::chrono::steady_clock::now();
    const auto max_timestamp = m_stage->get_max_absolute_timestamp();
    const auto& overall_result = m_stage->get_overall_result();
    const auto& length_formula = overall_result.length_formula;
    while (true)
    {
        {
            const std::lock_guard<std::mutex> guard(mutex);
            if (!is_playing)
                return;
        }
        const Math::timestamp_t timestamp = (static_cast<std::chrono::duration<Math::timestamp_t>>(std::chrono::steady_clock::now() - start)).count();
        const auto looped_timestamp = timestamp - std::floor(timestamp / max_timestamp) * max_timestamp;
        const auto info_at_time = m_stage->get_info_at_time(looped_timestamp);
        const auto counters_at_time = m_stage->get_counters_at_time(looped_timestamp);
        m_view->draw_chasis(length_formula, info_at_time.first);
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(2) << std::setw(2) << std::setfill('0');
            stream << "Time: " << looped_timestamp << " s, Velocity " << info_at_time.second << " m/s, Position: " << info_at_time.first << " m";
            ui->viewStats->setText(QString::fromStdString(stream.str()));
        }
        const size_t sensors_count = counters_at_time.size();
        for (size_t index = 0; index < sensors_count; ++index)
        {
            std::stringstream stream;
            stream << counters_at_time[index];
            ui->counterList->item(index)->setText(QString::fromStdString(stream.str()));
        }
        std::this_thread::sleep_for(sleep);
    }
}

