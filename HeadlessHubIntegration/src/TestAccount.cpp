/*
 * Copyright (c) 2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <math.h>

#include "TestAccount.hpp"

#include <QDebug>

TestAccount::TestAccount(UDSUtil* udsUtil, HubCache* hubCache) : HubAccount(udsUtil, hubCache)
{
	qDebug()  << "TestAccount::TestAccount " << udsUtil;

	_categoryId = 0;

    _name = "Test Account";
    _displayName = "Test Account";
    _description = "Test Account";
    _serverName = "ws.test.com";
    _iconFilename = "acc_uncolored.png";
    _lockedIconFilename = "acc_locked.png";
    _composeIconFilename = "acc.png";
    _supportsCompose = true;
    _supportsMarkRead = true;
    _supportsMarkUnread = true;
    _headlessTarget = "com.example.HeadlessHubIntegration";
    _appTarget = "com.example.HubIntegration";
    _cardTarget = "com.example.HubIntegration.card";
    _itemMimeType = "hub/vnd.test.item";  // mime type for hub items - if you change this, adjust invocation targets
                                            // to match and ensure this is unique for your application or you might invoke the wrong card
    _itemComposeIconFilename = "compose.png";
    _itemReadIconFilename = "itemRead.png";
    _itemUnreadIconFilename = "itemUnread.png";
    _markReadActionIconFilename = "itemMarkRead.png";
    _markUnreadActionIconFilename = "itemMarkUnread.png";

    // on device restart / update, it may be necessary to reload the Hub
    if (_udsUtil->reloadHub()) {
        _udsUtil->cleanupAccountsExcept(-1, _displayName);
        _udsUtil->initNextIds();
    }

    initialize();

    QVariantList categories;
    QVariantMap category;
    category["categoryId"] = 1; // categories are created with sequential category Ids starting at 1 so number your predefined categories
                                // accordingly
    category["name"] = "Inbox";
    category["parentCategoryId"] = 0; // default parent category ID for root categories
    categories << category;
    initializeCategories(categories);

    // reload existing hub items if required
    if (_udsUtil->reloadHub()) {
        repopulateHub();

        _udsUtil->resetReloadHub();
    }
}

TestAccount::~TestAccount()
{
}

qint64 TestAccount::accountId()
{
    return _accountId;
}

qint64 TestAccount::categoryId()
{
    return _categoryId;
}

void TestAccount::initializeCategories(QVariantList newCategories)
{
    HubAccount::initializeCategories(newCategories);

    if (_categoriesInitialized) {
        // initialize category ID - we are assuming that we only added one category
        QVariantList categories = _hubCache->categories();

        _categoryId = categories[0].toMap()["categoryId"].toLongLong();
    }
}
