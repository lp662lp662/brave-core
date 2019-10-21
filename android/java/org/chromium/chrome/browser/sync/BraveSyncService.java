/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.chromium.chrome.browser.sync;

import org.chromium.chrome.browser.sync.BraveSyncServiceObserver;

import java.util.ArrayList;

public class BraveSyncService {
    public static final String DELETE_RECORD = "2";

    public static final int NICEWARE_WORD_COUNT = 16;
    public static final int BIP39_WORD_COUNT = 24;

    private static BraveSyncService sInstance;
    private static final Object mLock = new Object();

    public static BraveSyncService getInstance() {
        synchronized (mLock) {
            if (sInstance == null) {
                sInstance = new BraveSyncService();
            }
        }
        return sInstance;
    }

    private BraveSyncService() {}

    class BookmarkInternal {
        public BookmarkInternal() {
        }
    }

    public class ResolvedRecordToApply implements Comparable<ResolvedRecordToApply> {
        public ResolvedRecordToApply(String objectId, String action,
                BookmarkInternal bookMarkInternal, String deviceName, String deviceId,
                long syncTime) {
        }

        public String mObjectId;
        public String mDeviceName;
        public String mDeviceId;

        @Override
        public int compareTo(ResolvedRecordToApply compare) {
            return 0;
        }
    }

    public ArrayList<ResolvedRecordToApply> GetAllDevices() {
        return new ArrayList<ResolvedRecordToApply>();
    }

    public void SetUpdateDeleteDeviceName(
            String action, String deviceName, String deviceId, String objectId) {}

    public void SetSyncEnabled(boolean syncEnabled) {}

    public void SetSyncBookmarksEnabled(boolean syncBookmarksEnabled) {}

    public void ResetSync() {}

    public void InitSync(boolean calledFromUIThread, boolean startNewChain) {}

    public void InitJSWebView(BraveSyncServiceObserver observer) {}

    public void GetSyncWords() {}

    public void GetNumber(String[] words) {}

    public void InterruptSyncSleep() {}

    public boolean IsSyncBookmarksEnabled() {
        return true;
    }
}
