
/******************************************************************************
   HotShots: Screenshot utility
   Copyright(C) 2011-2014  xbee@xbee.net

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *******************************************************************************/

#include "ItemRegistering.h"
#include "ItemFactory.h"

#include "EditorLineItem.h"
#include "EditorArrowItem.h"
#include "EditorTagItem.h"
#include "EditorEncircleItem.h"
#include "EditorPolylineItem.h"
#include "EditorPolygonItem.h"
#include "EditorEllipseItem.h"
#include "EditorRectangleItem.h"
#include "EditorBlurItem.h"
#include "EditorTextItem.h"
#include "EditorImageItem.h"
#include "EditorHighlighterItem.h"
#include "EditorMagnifierItem.h"
#include "EditorCurveItem.h"
#include "EditorCropItem.h"

int ItemRegistering::allRegister(ItemFactory *factoryInstance)
{
    static ItemFactoryTemplate<EditorLineItem> typeLine_;
    static ItemFactoryRegister a("EditorLineItem", &typeLine_, factoryInstance);

    static ItemFactoryTemplate<EditorArrowItem> typeArrow_;
    static ItemFactoryRegister b("EditorArrowItem", &typeArrow_, factoryInstance);

    static ItemFactoryTemplate<EditorTagItem> typeTag_;
    static ItemFactoryRegister c("EditorTagItem", &typeTag_, factoryInstance);

    static ItemFactoryTemplate<EditorEncircleItem> typeEncircle_;
    static ItemFactoryRegister d("EditorEncircleItem", &typeEncircle_, factoryInstance);

    static ItemFactoryTemplate<EditorPolylineItem> typePolyline_;
    static ItemFactoryRegister e("EditorPolylineItem", &typePolyline_, factoryInstance);

    static ItemFactoryTemplate<EditorPolygonItem> typePolygon_;
    static ItemFactoryRegister f("EditorPolygonItem", &typePolygon_, factoryInstance);

    static ItemFactoryTemplate<EditorEllipseItem> typeEllipse_;
    static ItemFactoryRegister g("EditorEllipseItem", &typeEllipse_, factoryInstance);

    static ItemFactoryTemplate<EditorRectangleItem> typeRectangle_;
    static ItemFactoryRegister h("EditorRectangleItem", &typeRectangle_, factoryInstance);

    static ItemFactoryTemplate<EditorBlurItem> typeBlur_;
    static ItemFactoryRegister i("EditorBlurItem", &typeBlur_, factoryInstance);

    static ItemFactoryTemplate<EditorTextItem> typeText_;
    static ItemFactoryRegister j("EditorTextItem", &typeText_, factoryInstance);

    static ItemFactoryTemplate<EditorImageItem> typeImage_;
    static ItemFactoryRegister k("EditorImageItem", &typeImage_, factoryInstance);

    static ItemFactoryTemplate<EditorHighlighterItem> typeHighlight_;
    static ItemFactoryRegister l("EditorHighlighterItem", &typeHighlight_, factoryInstance);

    static ItemFactoryTemplate<EditorMagnifierItem> typeMag_;
    static ItemFactoryRegister m("EditorMagnifierItem", &typeMag_, factoryInstance);

    static ItemFactoryTemplate<EditorCurveItem> typeCurve_;
    static ItemFactoryRegister n("EditorCurveItem", &typeCurve_, factoryInstance);

    static ItemFactoryTemplate<EditorCropItem> typeCrop_;
    static ItemFactoryRegister o("EditorCropItem", &typeCrop_, factoryInstance);

    return factoryInstance->types().size();
}
