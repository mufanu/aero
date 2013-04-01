#!/bin/sh
################################################################################
# This script backup drupal site
# Author: Mukhametdinov F, mufanu@gmail.com
################################################################################

BACKUPDIR="/var/www/backups"

DATE=`date +"%d.%m.%Y"`

/var/www/drush/drush archive-dump --destination=/var/www/backups/$DATE.tar.gz

#FROMROOT="portal@aero-express.ru"
#msg="From: $FROMROOT"
#To: mufanu@gmail.com
#Subject: Test
#Content-Type: text/plain; charset=koi8-r"

#echo -e "$msg" | /usr/sbin/sendamil -f $FROMROOT "mufanu@gmail.com"