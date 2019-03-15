//=============================================================================
/// Copyright (c) 2016-2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implementation for a class that contains colored legends
//=============================================================================

#include "ColoredLegendScene.h"

#include <QDebug>
#include <QGraphicsRectItem>
#include <QGraphicsView>

#include <../Scaling/ScalingManager.h>
#include <../Util/QtUtil.h>

static const int s_TEXT_SQUARE_OFFSET_X = 20;
static const int s_TEXT_SQUARE_OFFSET_Y = 2;

// Colored legends definitions
static const int gs_LEGENDS_BASE_HEIGHT = 15;

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param pParentView The view displaying this widget
/// \param fontPixelSize The pixel size of the font
//-----------------------------------------------------------------------------
ColoredLegendScene::ColoredLegendScene()
{
    m_pScene = nullptr;
}

//-----------------------------------------------------------------------------
/// Explicit constructor
/// \param pParentView The view displaying this widget
/// \param fontPixelSize The pixel size of the font
//-----------------------------------------------------------------------------
ColoredLegendScene::~ColoredLegendScene()
{
    Clear();
}

//-----------------------------------------------------------------------------
/// Add a new box with a description beside it
/// \param color The color of the box
/// \param desc The legend description
//-----------------------------------------------------------------------------
void ColoredLegendScene::AddColorLegendItem(const QColor& color, const QString& desc)
{
    if (m_pScene != nullptr)
    {
        m_legendMode = LEGEND_MODE_COLOR;

        ColorLegendItem legendItem = {};
        legendItem.pRectItem = new QGraphicsRectItem();
        legendItem.textInfo.pTextItem = new QGraphicsTextItem(desc);
        legendItem.textInfo.textWidth = QtCommon::QtUtil::GetTextWidth(m_font, desc);

        legendItem.textInfo.pTextItem->setPos(0, 0);
        legendItem.textInfo.pTextItem->setFont(m_font);

        legendItem.pRectItem->setRect(0, 0, gs_LEGENDS_BASE_HEIGHT * ScalingManager::Get().GetScaleFactor(), gs_LEGENDS_BASE_HEIGHT * ScalingManager::Get().GetScaleFactor());
        legendItem.pRectItem->setBrush(color);
        legendItem.pRectItem->setPen(color);

        m_colorLegends.push_back(legendItem);

        m_pScene->addItem(legendItem.pRectItem);
        m_pScene->addItem(legendItem.textInfo.pTextItem);

        Update();
    }
}

//-----------------------------------------------------------------------------
/// Add a string-only legend
/// \param desc The legend description
//-----------------------------------------------------------------------------
void ColoredLegendScene::AddTextLegendItem(const QString& str)
{
    if (m_pScene != nullptr)
    {
        m_legendMode = LEGEND_MODE_TEXT;

        TextLegendItem legendItem = {};
        legendItem.pTextItem = new QGraphicsTextItem(str);
        legendItem.textWidth = QtCommon::QtUtil::GetTextWidth(m_font, str);

        legendItem.pTextItem->setPos(0, 0);
        legendItem.pTextItem->setFont(m_font);

        m_textLegends.push_back(legendItem);

        m_pScene->addItem(legendItem.pTextItem);

        Update();
    }
}

//-----------------------------------------------------------------------------
/// HWipe the scene clean
//-----------------------------------------------------------------------------
void ColoredLegendScene::Clear()
{
    if (m_pScene != nullptr)
    {
        for (int i = 0; i < m_colorLegends.size(); i++)
        {
            m_pScene->removeItem(m_colorLegends[i].pRectItem);
            m_pScene->removeItem(m_colorLegends[i].textInfo.pTextItem);
            delete m_colorLegends[i].pRectItem;
            delete m_colorLegends[i].textInfo.pTextItem;
        }

        m_colorLegends.clear();

        for (int i = 0; i < m_textLegends.size(); i++)
        {
            m_pScene->removeItem(m_textLegends[i].pTextItem);
            delete m_textLegends[i].pTextItem;
        }

        m_textLegends.clear();
    }
}

//-----------------------------------------------------------------------------
/// Handle cases when the user resizes the window.
//-----------------------------------------------------------------------------
void ColoredLegendScene::ResizeEvent(const QSize& newSize)
{
    if (m_pScene != nullptr)
    {
        m_width = newSize.width();
        m_height = newSize.height();

        Update();
    }
}

//-----------------------------------------------------------------------------
/// Update item positions
//-----------------------------------------------------------------------------
void ColoredLegendScene::Update()
{
    if (m_pScene != nullptr)
    {
        const int yPosTop = (m_height / 2) * -1;

        m_pScene->setSceneRect((m_width / 2) * -1, yPosTop, m_width, m_height);

        const double scaleFactor = ScalingManager::Get().GetScaleFactor();

        int xPos = m_pScene->sceneRect().x() + (m_width % 2 == 0 ? 0 : 1);

        if (m_legendMode == LEGEND_MODE_COLOR)
        {
            for (int i = 0; i < m_colorLegends.size(); i++)
            {
                const ColorLegendItem& item = m_colorLegends[i];

                item.pRectItem->setPos(xPos, yPosTop);
                item.textInfo.pTextItem->setPos(xPos + (gs_LEGENDS_BASE_HEIGHT * scaleFactor), yPosTop - (s_TEXT_SQUARE_OFFSET_Y * scaleFactor));

                xPos += (gs_LEGENDS_BASE_HEIGHT * scaleFactor) + item.textInfo.textWidth + (s_TEXT_SQUARE_OFFSET_X * scaleFactor);
            }
        }
        else if (m_legendMode == LEGEND_MODE_TEXT)
        {
            for (int i = 0; i < m_textLegends.size(); i++)
            {
                m_textLegends[i].pTextItem->setPos(xPos, yPosTop);

                xPos += m_textLegends[i].textWidth + s_TEXT_SQUARE_OFFSET_X;
            }
        }
    }
}
