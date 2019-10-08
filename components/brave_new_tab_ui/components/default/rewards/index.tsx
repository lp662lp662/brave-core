/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import createWidget from '../widget/index'
import { convertBalance, tipsTotal } from '../../../../brave_rewards/resources/page/utils'
import { getLocale } from '../../../../common/locale'

import {
  WidgetWrapper,
  BatIcon,
  RewardsTitle,
  Footer,
  ServiceText,
  ServiceLink,
  LearnMoreText,
  PreOptInInfo,
  Title,
  SubTitle,
  PreOptInAction,
  TurnOnButton,
  AmountItem,
  AmountInformation,
  AmountDescription,
  Amount,
  ConvertedAmount,
  LearnMoreLink,
  TurnOnAdsButton
} from './style'
import Notification from './notification'
import { BatColorIcon } from 'brave-ui/components/icons'

export interface RewardsProps {
  enabledAds: boolean
  enabledMain: boolean
  balance: NewTab.RewardsBalance
  grants: NewTab.GrantRecord[]
  reports: Record<string, NewTab.RewardsReport>
  walletCreated: boolean
  walletCreating: boolean
  walletCreateFailed: boolean
  walletCorrupted: boolean
  adsEstimatedEarnings: number
  onCreateWallet: () => void
  onEnableAds: () => void
  onEnableRewards: () => void
  onDismissNotification: (id: string) => void
}

class Rewards extends React.PureComponent<RewardsProps, {}> {
  getTotalContributions = () => {
    const currentTime = new Date()
    const reportKey = `${currentTime.getFullYear()}_${currentTime.getMonth() + 1}`
    const report: NewTab.RewardsReport = this.props.reports[reportKey]

    if (report) {
      return tipsTotal(report)
    }

    return '0.0'
  }

  getButtonText = (isAds: boolean = false) => {
    if (isAds) {
      return getLocale('turnOnAds')
    }

    const {
      walletCreating,
      walletCreateFailed,
      walletCorrupted
    } = this.props

    if (walletCreateFailed || walletCorrupted) {
      return getLocale('walletFailedButton')
    }

    if (walletCreating) {
      return getLocale('rewardsWidgetTurningOn')
    }

    return getLocale('rewardsWidgetTurnOnRewards')
  }

  optInAction = (hasEnabled: boolean) => {
    if (hasEnabled) {
      this.props.onEnableRewards()
    } else {
      this.props.onCreateWallet()
    }
  }

  renderPreOptIn = () => {
    const {
      enabledMain,
      walletCreated
    } = this.props

    if (enabledMain && walletCreated) {
      return null
    }

    const hasEnabled = !enabledMain && walletCreated

    return (
      <>
        <PreOptInInfo>
          <Title>
            {
              hasEnabled
              ? getLocale('rewardsWidgetReEnableTitle')
              : getLocale('rewardsWidgetEnableTitle')
            }
          </Title>
          <SubTitle>
            {
              hasEnabled
              ? getLocale('rewardsWidgetReEnableSubTitle')
              : getLocale('rewardsWidgetEnableSubTitle')
            }
          </SubTitle>
        </PreOptInInfo>
        <PreOptInAction>
          <TurnOnButton
            data-test-id={'optInAction'}
            onClick={this.optInAction.bind(this, hasEnabled)}
          >
            {this.getButtonText()}
          </TurnOnButton>
        </PreOptInAction>
      </>
    )
  }

  renderAmountItem = (type: string) => {
    const {
      balance,
      enabledAds,
      onEnableAds,
      adsEstimatedEarnings
    } = this.props

    const rates = balance.rates || {}
    const showEnableAds = type === 'ads' && !enabledAds
    const amount = type === 'ads'
      ? adsEstimatedEarnings.toFixed(1)
      : this.getTotalContributions()
    const converted = convertBalance(amount, rates)

    return (
      <AmountItem>
        {
          showEnableAds
          ? <TurnOnAdsButton onClick={onEnableAds}>
              {this.getButtonText(true)}
            </TurnOnAdsButton>
          : null
        }
        {
          !showEnableAds
          ? <AmountInformation data-test-id={`widget-amount-total-${type}`}>
              <Amount>{amount}</Amount>
              <ConvertedAmount>
                {`${getLocale('bat')} (${converted} USD)`}
              </ConvertedAmount>
            </AmountInformation>
          : null
        }
        <AmountDescription>
          {
            type === 'ads'
            ? getLocale('rewardsWidgetEstimatedEarnings')
            : getLocale('rewardsWidgetMonthlyTips')
          }
        </AmountDescription>
      </AmountItem>
    )
  }

  renderRewardsInfo = () => {
    const {
      enabledMain,
      walletCreated
    } = this.props

    if (!enabledMain || !walletCreated) {
      return null
    }

    return (
      <div data-test-id2={'enableMain'}>
        {this.renderAmountItem('ads')}
        {this.renderAmountItem('tips')}
      </div>
    )
  }

  renderLearnMore = () => {
    return (
      <LearnMoreText>
        <LearnMoreLink href={'chrome://rewards'}>
          {getLocale('learnMore')}
        </LearnMoreLink>
        {getLocale('rewardsWidgetAboutRewards')}
      </LearnMoreText>
    )
  }

  renderPrivacyPolicy = () => {
    return (
      <>
        <ServiceText>
          {getLocale('rewardsWidgetServiceText')} <ServiceLink href={'https://brave.com/terms-of-use'}>{getLocale('termsOfService')}</ServiceLink> {getLocale('and')} <ServiceLink href={'https://brave.com/privacy#rewards'}>{getLocale('privacyPolicy')}</ServiceLink>.
        </ServiceText>
      </>
    )
  }

  renderNotifications = () => {
    const { grants, onDismissNotification } = this.props

    return (
      <>
        {grants.map((grant: NewTab.GrantRecord, index) => {
          return (
            <Notification
              grant={grant}
              key={`notification-${index}`}
              onDismissNotification={onDismissNotification}
            />
          )
        })}
      </>
    )
  }

  render () {
    const {
      enabledMain,
      walletCreated
    } = this.props

    return (
      <WidgetWrapper>
        <>
          <BatIcon>
            <BatColorIcon />
          </BatIcon>
          <RewardsTitle>
            {getLocale('braveRewards')}
          </RewardsTitle>
        </>
        {this.renderPreOptIn()}
        {this.renderRewardsInfo()}
        <Footer>
          {
            enabledMain && walletCreated
            ? this.renderLearnMore()
            : this.renderPrivacyPolicy()
          }
        </Footer>
        {
          enabledMain
          ? this.renderNotifications()
          : null
        }
      </WidgetWrapper>
    )
  }
}

export const RewardsWidget = createWidget(Rewards)
