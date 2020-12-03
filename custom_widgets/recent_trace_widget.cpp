//=============================================================================
/// Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief A widget which encapsulates a single recent trace selection.
//=============================================================================

#include "recent_trace_widget.h"

#include <QVBoxLayout>
#include <QSignalMapper>
#include <QEvent>

#include <time.h>

#include "utils/common_definitions.h"
#include "utils/qt_util.h"
#include "utils/scaling_manager.h"

/// String buffer size.
static const int kBufferSize = 64;

RecentTraceWidget::RecentTraceWidget(QWidget* parent)
    : RecentTraceMiniWidget(parent)
{
    delete_signal_mapper_      = new QSignalMapper(this);
    delete_button_             = new ScaledPushButton(this);
    open_file_location_button_ = new ScaledPushButton(this);

    access_label_ = new ScaledLabel(this);

    SetupFonts(this->font().pointSizeF());

    // Additional settings for path button
    QSizePolicy policy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setSizePolicy(policy);

    // Set the size policy for various labels
    access_label_->setSizePolicy(policy);
    delete_button_->setSizePolicy(policy);
    delete_button_->setStyleSheet(kLinkButtonStylesheet);
    delete_button_->setCursor(Qt::PointingHandCursor);
    delete_button_->setText("Remove from list");

    open_file_location_button_->setSizePolicy(policy);
    open_file_location_button_->setStyleSheet(kLinkButtonStylesheet);
    open_file_location_button_->setCursor(Qt::PointingHandCursor);
    open_file_location_button_->setText("Open file location");
}

RecentTraceWidget::~RecentTraceWidget()
{
    delete delete_signal_mapper_;
}

void RecentTraceWidget::changeEvent(QEvent* event)
{
    if (event != nullptr && event->type() == QEvent::Type::FontChange)
    {
        SetupFonts(this->font().pointSizeF());
    }
}

void RecentTraceWidget::AddCustomWidgets()
{
    widget_layout_->addWidget(access_label_);
}

QString RecentTraceWidget::GetLastAccessedTime(const RecentFileData& file)
{
    // Get file access time
    const time_t time_accessed            = file.accessed.toLongLong();
    char         time_string[kBufferSize] = {};
#ifdef _WIN32
    ctime_s(time_string, kBufferSize, &time_accessed);

    // ctime_s produces a string with '\n' at the end, we need to remove this
    for (int i = 0; i < kBufferSize; i++)
    {
        // Assuming there will only be one newline, so stop after seeing the first one
        if (time_string[i] == '\n')
        {
            time_string[i] = '\0';
            break;
        }
    }

#else
    const char* time_value = ctime(&time_accessed);
    strcpy(time_string, time_value);
#endif

    return QString("last accessed on %1").arg(time_string);
}

void RecentTraceWidget::SetupFonts(qreal font_size)
{
    QFont path_label_font;
    QFont small_font;
    QFont option_link_font;

    path_label_font.setUnderline(true);
    option_link_font.setUnderline(true);

    qreal path_label_font_size = std::max<qreal>(1.0, font_size);
    qreal small_font_size      = std::max<qreal>(1.0, font_size - 2.0);

    path_label_font.setPointSizeF(path_label_font_size);
    small_font.setPointSizeF(small_font_size);
    option_link_font.setPointSizeF(small_font_size);

    // Override file path font
    path_button_->setFont(path_label_font);

    // Set label fonts
    access_label_->setFont(small_font);
    delete_button_->setFont(option_link_font);
    open_file_location_button_->setFont(option_link_font);
}

void RecentTraceWidget::SetOpenFileLocationText(const QString& label)
{
    open_file_location_button_->setText(label);
}

void RecentTraceWidget::Initialize(const RecentFileData& file)
{
    SetRecentFileData(file);
}

void RecentTraceWidget::SetRecentFileData(const RecentFileData& file)
{
    RecentTraceMiniWidget::SetFile(file);

    // Add widgets to the layout
    widget_layout_->addItem(new QSpacerItem(5, ScalingManager::Get().Scaled(5), QSizePolicy::Ignored, QSizePolicy::Fixed));
    widget_layout_->setSpacing(ScalingManager::Get().Scaled(2));

    // Add any widgets from derived classes
    AddCustomWidgets();

    // Add an expanding spacer at the bottom to keep the items forced upwards
    widget_layout_->addItem(new QSpacerItem(1, 1, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    widget_layout_->setSizeConstraint(QLayout::SetMinimumSize);

    option_buttons_layout_ = new QHBoxLayout(widget_layout_->widget());
    option_buttons_layout_->setSpacing(ScalingManager::Get().Scaled(20));
    option_buttons_layout_->addWidget(delete_button_);
    option_buttons_layout_->addWidget(open_file_location_button_);
    option_buttons_layout_->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
    widget_layout_->addLayout(option_buttons_layout_);

    // Get file parameters
    QString path = file.path;

    // Override the path label text
    path_button_->setText(path);
    path_button_->setToolTip("");
    access_label_->setText(GetLastAccessedTime(file));

    // Connect path label clicked() signal to this widgets clicked(QString) signal
    // with the delete button as the parameter
    delete_signal_mapper_->setMapping(delete_button_, path);
    connect(delete_button_, SIGNAL(clicked()), delete_signal_mapper_, SLOT(map()));
    connect(delete_signal_mapper_, SIGNAL(mapped(QString)), this, SIGNAL(clickedDelete(QString)));

    // Connect clicked() signal for "Open file location" button.
    connect(open_file_location_button_, &QPushButton::clicked, this, &RecentTraceWidget::OpenFileLocation);
}

void RecentTraceWidget::OpenFileLocation(bool checked)
{
    Q_UNUSED(checked);
    if (QtCommon::QtUtils::BrowseToFile(GetPath()) == false)
    {
        emit OpenFileLocationFailed(GetPath());
    }
}

QString RecentTraceWidget::GetPath() const
{
    return path_button_->text();
}
