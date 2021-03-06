/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

/**
 * This class is experimental, and as such has not been tested.
 * Please help make it more robust by contributing fixes if you find issues
 **/

#import "alljoyn/time/AJTMTimeServiceServerAlarmBase.h"
#import "alljoyn/time/AJTMTimeServiceServerAlarm.h"

@interface SampleAlarm : AJTMTimeServiceServerAlarmBase <AJTMTimeServiceServerAlarm>

-(id)init;

#pragma mark AJTMTimeServiceServerAlarm protocol methods
/**
 * Returns TimeServiceSchedule of this Alarm
 *
 * @return Alarm
 */
-(AJTMTimeServiceSchedule*)schedule;

/**
 * Set TimeServiceSchedule to this Alarm object
 *
 * @param schedule TimeServiceSchedule
 * @return Status ER_OK If succeeded to set the Schedule, otherwise, if this Alarm does not support
 * one of the Schedule values, return an appropriate failure status.
 */
-(QStatus)setSchedule:(AJTMTimeServiceSchedule *) schedule;

/**
 * Returns Alarm title.
 *
 * @return Optional textual description of what this Alarm is
 */
-(NSString*)title;

/**
 * Set title to this Alarm.
 *
 * @param title Alarm title.
 * Title is an optional textual description of what this Alarm is set for.
 */
-(void)setTitle:(NSString*) title;

/**
 * Returns whether or not this Alarm is enabled
 *
 * @return Is Alarm enabled
 */
-(bool)isEnabled;

/**
 * Set whether or not this Alarm is enabled
 *
 * @param enabled Is Alarm enabled
 */
-(void)setEnabled:(bool) enabled;

@end
