/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { bindActionCreators, Dispatch } from 'redux'
import { connect } from 'react-redux'

// Components
import SettingsPage from './settingsPage'

// Utils
import * as rewardsActions from '../actions/rewards_actions'
import { WelcomePage } from '../../ui/components'

interface Props extends Rewards.ComponentProps {
}

interface State {
  creating: boolean
}

export class App extends React.Component<Props, State> {
  constructor (props: Props) {
    super(props)
    this.state = {
      creating: false
    }
  }

  componentDidMount () {
    if (!this.props.rewardsData.walletCreated) {
      this.actions.checkWalletExistence()
    }

    if (this.props.rewardsData.enabledMain &&
        !this.props.rewardsData.enabledAdsMigrated) {
      const { adsEnabled, adsIsSupported } = this.props.rewardsData.adsData

      if (adsIsSupported) {
        this.props.actions.onAdsSettingSave('adsEnabledMigrated', adsEnabled)
      }
    }

    this.handleGrantNotification()
  }

  componentDidUpdate (prevProps: Props, prevState: State) {
    if (
      this.state.creating &&
      !prevProps.rewardsData.walletCreateFailed &&
      this.props.rewardsData.walletCreateFailed
    ) {
      this.setState({
        creating: false
      })
    }
  }

  handleGrantNotification = () => {
    const hash = window && window.location && window.location.hash

    if (!hash) {
      return
    }

    if (!hash.startsWith('#grant_')) {
      return
    }

    this.actions.getGrantCaptcha(hash.split('#grant_')[1])
  }

  onCreateWalletClicked = () => {
    this.actions.createWallet()
    this.setState({
      creating: true
    })
  }

  openTOS () {
    window.open('https://brave.com/terms-of-use', '_blank')
  }

  openPrivacyPolicy () {
    window.open('https://brave.com/privacy#rewards', '_blank')
  }

  get actions () {
    return this.props.actions
  }

  render () {
    const { walletCreated, walletCreateFailed } = this.props.rewardsData

    let props: {onReTry?: () => void} = {}

    if (walletCreateFailed) {
      props = {
        onReTry: this.onCreateWalletClicked
      }
    }

    return (
      <div id='rewardsPage'>
        {
          !walletCreated
          ? <WelcomePage
            onTOSClick={this.openTOS}
            onPrivacyClick={this.openPrivacyPolicy}
            optInAction={this.onCreateWalletClicked}
            creating={this.state.creating}
            {...props}
          />
          : null
        }
        {
          walletCreated
          ? <SettingsPage />
          : null
        }
      </div>
    )
  }
}

export const mapStateToProps = (state: Rewards.ApplicationState) => ({
  rewardsData: state.rewardsData
})

export const mapDispatchToProps = (dispatch: Dispatch) => ({
  actions: bindActionCreators(rewardsActions, dispatch)
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(App)
