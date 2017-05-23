
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

#ifndef _ITEMFACTORY_H
#define _ITEMFACTORY_H

#include <QtCore/QMap>
#include <QtCore/QHash>
#include <QtCore/QString>

class BaseItem;
class QGraphicsItem;

class ItemFactoryInterface
{
public:

    virtual BaseItem * create(QGraphicsItem *parent) = 0;
};

template <typename T>
class ItemFactoryTemplate : public ItemFactoryInterface
{
    BaseItem * create(QGraphicsItem *parent = 0)
    {
        return new T(parent);
    }
};

struct ItemFactoryData
{
public:

    ItemFactoryData() : factory(0){}
    ItemFactoryData(ItemFactoryInterface *f) : factory(f){}
    ItemFactoryInterface *factory;
};

class ItemFactory
{
public:

    BaseItem * create(const QString &type, QGraphicsItem *parent = 0)
    {
        if (types_.contains(type) == false)
            return NULL;

        return types_.value(type).factory->create(parent);
    }

    void registerType(const QString &type, ItemFactoryInterface *factory)
    {
        if (types_.contains(type) == false)
            types_.insert( type, ItemFactoryData (factory) );
    }

    QList<QString> types()
    {
        return types_.keys();
    }

private:

    QMap<QString, ItemFactoryData> types_;
};

class ItemFactoryRegister
{
public:

    ItemFactoryRegister(const QString &       type,
                        ItemFactoryInterface *factory,
                        ItemFactory *         instance)
    {
        instance->registerType(type, factory);
    }
};

#endif // _ITEMFACTORY_H
