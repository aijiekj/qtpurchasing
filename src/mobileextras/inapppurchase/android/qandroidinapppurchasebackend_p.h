/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the Qt Mobile Extras Add-on.
**
** $QT_BEGIN_LICENSE$
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QANDROIDINAPPPURCHASEBACKEND_P_H
#define QANDROIDINAPPPURCHASEBACKEND_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qinapppurchasebackend_p.h"
#include "qinappproduct.h"
#include "qinapptransaction.h"

#include <QtCore/qmutex.h>
#include <QtCore/qset.h>
#include <QtAndroidExtras/qandroidjniobject.h>
#include <QtAndroidExtras/qandroidactivityresultreceiver.h>

QT_BEGIN_NAMESPACE

class QAndroidInAppProduct;
class QAndroidInAppPurchaseBackend : public QInAppPurchaseBackend, public QAndroidActivityResultReceiver
{
    Q_OBJECT
public:
    explicit QAndroidInAppPurchaseBackend(QObject *parent = 0);

    void initialize();
    bool isReady() const;

    void queryProduct(QInAppProduct::ProductType productType, const QString &identifier);
    void restorePurchases();

    void setPlatformProperty(const QString &propertyName, const QString &value);

    void purchaseProduct(QAndroidInAppProduct *product);

    void consumeTransaction(const QString &purchaseToken);
    void registerFinalizedUnlockable(const QString &identifier);

    // Callbacks from Java
    Q_INVOKABLE void registerQueryFailure(const QString &productId);
    Q_INVOKABLE void registerProduct(const QString &productId, const QString &price);
    Q_INVOKABLE void registerPurchased(const QString &identifier, const QString &signature, const QString &data, const QString &purchaseToken, const QString &orderId);
    Q_INVOKABLE void purchaseSucceeded(int requestCode, const QString &signature, const QString &data, const QString &purchaseToken, const QString &orderId);
    Q_INVOKABLE void purchaseFailed(int requestCode);
    Q_INVOKABLE void registerReady();

    void handleActivityResult(int requestCode, int resultCode, const QAndroidJniObject &data);

    QString finalizedUnlockableFileName() const;

private:
    void checkFinalizationStatus(QInAppProduct *product);
    bool transactionFinalizedForProduct(QInAppProduct *product);
    void purchaseFailed(QInAppProduct *product);

    struct PurchaseInfo
    {
        PurchaseInfo(const QString &signature_, const QString &data_, const QString &purchaseToken_, const QString &orderId_)
            : signature(signature_)
            , data(data_)
            , purchaseToken(purchaseToken_)
            , orderId(orderId_)
        {
        }

        QString signature;
        QString data;
        QString purchaseToken;
        QString orderId;
    };

    mutable QMutex m_mutex;
    bool m_isReady;
    QAndroidJniObject m_javaObject;
    QHash<QString, QInAppProduct::ProductType> m_productTypeForPendingId;
    QHash<QString, PurchaseInfo> m_infoForPurchase;
    QSet<QString> m_finalizedUnlockableProducts;
    QHash<QString, QInAppProduct *> m_pendingPurchaseForIdentifier;
    QHash<int, QInAppProduct *> m_activePurchaseRequests;
};

QT_END_NAMESPACE

#endif // QANDROIDINAPPPURCHASEBACKEND_P_H
