/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import {
  Title,
  SubTitle,
  CloseIcon,
  Content,
  NotificationWrapper,
  NotificationButton
} from './style'
import { CloseStrokeIcon } from 'brave-ui/components/icons'
import { getLocale } from '../../../../common/locale'

interface NotificationProps {
  grant: NewTab.GrantRecord
  onDismissNotification: (id: string) => void
}

export default class RewardsNotification extends React.PureComponent<NotificationProps, {}> {

  dismissNotification = () => {
    this.props.onDismissNotification(this.props.grant.promotionId)
  }

  onNotificationAction = () => {
    this.dismissNotification()
    window.open(`chrome://rewards#g_${this.props.grant.promotionId}`, '_blank')
  }

  render () {
    return (
      <NotificationWrapper>
        <CloseIcon onClick={this.dismissNotification}>
          <CloseStrokeIcon />
        </CloseIcon>
        <Content>
          <Title>
            {getLocale('rewardsWidgetNotificationTitle')}
          </Title>
          <SubTitle>
            {
              this.props.grant.type === 'ads'
              ? getLocale('rewardsWidgetNotificationTextAds')
              : getLocale('rewardsWidgetNotificationTextUGP')
            }
          </SubTitle>
          <NotificationButton onClick={this.onNotificationAction}>
            {getLocale('claimMyRewards')}
          </NotificationButton>
        </Content>
      </NotificationWrapper>
    )
  }
}
