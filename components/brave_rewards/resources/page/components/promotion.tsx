/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { bindActionCreators, Dispatch } from 'redux'
import { connect } from 'react-redux'

// Components
import {
  GrantWrapper,
  GrantCaptcha,
  GrantComplete
} from '../../ui/components'
import GrantClaim, { Type } from '../../ui/components/grantClaim'

// Utils
import * as rewardsActions from '../actions/rewards_actions'
import { getLocale } from '../../../../common/locale'
import GrantError from '../../ui/components/grantError'

interface State {
  promotionShow: boolean
}

interface Props extends Rewards.ComponentProps {
  promotion: Rewards.Promotion
  onlyAnonWallet?: boolean
}

// TODO add local when we know what we will get from the server
class Promotion extends React.Component<Props, State> {
  constructor (props: Props) {
    super(props)
    this.state = {
      promotionShow: true
    }
  }

  get actions () {
    return this.props.actions
  }

  onShow = (promotionId?: string) => {
    this.actions.claimPromotion(promotionId)
  }

  onHide = () => {
    this.actions.onResetGrant()
  }

  onFinish = () => {
    this.setState({
      promotionShow: false
    })
    this.actions.onDeleteGrant()
  }

  onSolution = (x: number, y: number) => {
    this.actions.attestPromotion(x, y)
  }

  getCaptcha = () => {
    const { promotion } = this.props

    if (!promotion) {
      return
    }

    if (promotion.status === 'grantGone') {
      return (
        <GrantWrapper
          onClose={this.onFinish}
          title={getLocale('grantGoneTitle')}
          text={''}
        >
          <GrantError
            buttonText={getLocale('grantGoneButton')}
            text={getLocale('grantGoneText')}
            onButtonClick={this.onFinish}
          />
        </GrantWrapper>
      )
    }

    if (promotion.status === 'grantAlreadyClaimed') {
      return (
        <GrantWrapper
          onClose={this.onFinish}
          title={getLocale('grantGoneTitle')}
          text={''}
        >
          <GrantError
            buttonText={getLocale('grantGoneButton')}
            text={getLocale('grantAlreadyClaimedText')}
            onButtonClick={this.onFinish}
          />
        </GrantWrapper>
      )
    }

    if (promotion.status === 'generalError') {
      return (
        <GrantWrapper
          onClose={this.onHide}
          title={getLocale('grantGeneralErrorTitle')}
          text={''}
        >
          <GrantError
            buttonText={getLocale('grantGeneralErrorButton')}
            text={getLocale('grantGeneralErrorText')}
            onButtonClick={this.onHide}
          />
        </GrantWrapper>
      )
    }

    if (!promotion.captchaImage || !promotion.hint) {
      return
    }

    return (
      <GrantWrapper
        onClose={this.onHide}
        title={promotion.status === 'wrongPosition' ? getLocale('notQuite') : getLocale('almostThere')}
        text={getLocale('proveHuman')}
      >
        <GrantCaptcha
          onSolution={this.onSolution}
          captchaImage={promotion.captchaImage}
          hint={promotion.hint}
          isWindows={navigator.platform === 'Win32'}
        />
      </GrantWrapper>
    )
  }

  getFinish = (type: string, tokens: string, date: string) => {
    let title = getLocale('grantFinishTitleUGP')
    let text = getLocale('grantFinishTextUGP')
    let tokenTitle = getLocale('grantFinishTokenUGP')
    const { onlyAnonWallet } = this.props

    if (type === 'ads') {
      title = getLocale('grantFinishTitleAds')
      text = getLocale('grantFinishTextAds')
      tokenTitle = getLocale('grantFinishTokenAds')
    }

    return (
      <GrantWrapper
        data-test-id={'grantWrapper'}
        onClose={this.onFinish}
        title={title}
        text={text}
      >
        <GrantComplete
          onClose={this.onFinish}
          amount={tokens}
          date={date}
          testId={'newTokenGrant'}
          tokenTitle={tokenTitle}
          onlyAnonWallet={onlyAnonWallet}
        />
      </GrantWrapper>
    )
  }

  convertPromotionTypesToType = (type: Rewards.PromotionTypes) => {
    switch (type) {
      case Rewards.PromotionTypes.ADS: {
        return 'ads'
      }
      default: {
        return 'ugp'
      }
    }
  }

  render () {
    const { promotion } = this.props

    if (!promotion) {
      return null
    }

    let type = 'ugp'
    let promoId
    let tokens = '0.0'
    let date = ''

    if (promotion.type) {
      type = this.convertPromotionTypesToType(promotion.type)
    }
    if (promotion.promotionId) {
      promoId = promotion.promotionId
    }
    if (promotion.amount) {
      tokens = promotion.amount.toFixed(1)
    }

    if (promotion.type !== 1) { // Rewards.PromotionTypes.ADS
      date = new Date(promotion.expiresAt).toLocaleDateString()
    }

    return (
      <>
        {
          this.state.promotionShow && type
            ? <GrantClaim type={type as Type} onClaim={this.onShow.bind(this, promoId)} testId={'claimGrant'}/>
            : null
        }
        {
          !promotion.expiresAt
            ? this.getCaptcha()
            : null
        }
        {
          promotion.expiresAt
            ? this.getFinish(type, tokens, date)
            : null
        }
      </>
    )
  }
}

const mapStateToProps = (state: Rewards.ApplicationState) => ({
  rewardsData: state.rewardsData
})

const mapDispatchToProps = (dispatch: Dispatch) => ({
  actions: bindActionCreators(rewardsActions, dispatch)
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Promotion)