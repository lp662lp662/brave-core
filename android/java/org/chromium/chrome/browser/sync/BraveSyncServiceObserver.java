/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.chromium.chrome.browser.sync;

/**
 * Allows monitoring of JavaScript results.
 */
public interface BraveSyncServiceObserver {
    /**
     * Informs when the provided code words are incorrect
     */
    public void onSyncError(String message);

    /**
     * Informs when the seed is received
     */
    public void onSeedReceived(String seed, boolean fromCodeWords, boolean afterInitialization);

    /**
     * Informs when the code words are received
     */
    public void onHaveSyncWords(String[] syncWords);

    /**
     * Informs when the list of devices is available
     */
    public void onDevicesAvailable();

    /**
     * Informs when the sync is reset
     */
    public void onResetSync();

    /**
     * Returns true if sync settings with devices list is currently shown
     */
    public boolean shouldLoadDevices();
}
