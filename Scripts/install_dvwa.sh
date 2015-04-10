#/bin/bash

echo "###########################################"
echo "# Damn Vulnerable Web App Installer Script #"
echo "#          For Kali Linux                  #"
echo "############################################"
echo " By: Josh Stroschein"
echo " Original Script based off of: http://theunl33t.blogspot.com/2011/08/script-to-download-configure-and-launch.html"
echo -n "[*] Changing directory to /var/www..."
cd /var/www > /dev/null
echo "Done!"

echo -n "[*] Removing default index.html..."
rm index.html >> /dev/null 2>&1
echo "Done!"

echo -n "[*] Changing to TMP Directory..."
cd /tmp
echo "Done!"

echo -n "[*] Downloading DVWA..."
wget https://github.com/RandomStorm/DVWA/archive/v1.0.8.zip >> /dev/null 2>&1
echo "Done!"

echo -n "[*] Unzipping DVWA..."
unzip v1.0.8.zip > /dev/null
echo "Done!\n"

echo -n "[*] Deleting the zip file..."
rm v1.0.8.zip > /dev/null
echo "Done!"

echo -n "[*] Copying DVWA to /var/www..."
cp -R DVWA-1.0.8/* /var/www > /dev/null
echo "Done!"

echo -n "[*] Clearing TMP Directory..."
rm -R DVWA-1.0.8 > /dev/null
echo "Done!"

echo -n "[*] Enabling Remote include in php.ini..."
cp /etc/php5/apache2/php.ini /etc/php5/apache2/php.ini1
sed -e 's/allow_url_include = Off/allow_url_include = On/' /etc/php5/apache2/php.ini1 > /etc/php5/apache2/php.ini
rm /etc/php5/apache2/php.ini1
echo "Done!"

echo -n "[*] Enabling write permissions to /var/www/hackable/upload..."
chmod 777 /var/www/hackable/uploads/
echo "Done!"

echo -n "[*] Starting Apache Web Server..."
service apache2 start >> /dev/null 2>&1
echo "Done!"

echo -n "[*] Starting MySQL..."
service mysql start >> /dev/null 2>&1
echo "Done!"

echo -n "[*] Updating Config File..."
cp /var/www/config/config.inc.php /var/www/config/config.inc.php1
sed -e 's/p@ssw0rd//' /var/www/config/config.inc.php1 > /var/www/config/config.inc.php
rm /var/www/config/config.inc.php1
echo "Done!"

echo -n "[*] Updating Database..."
wget --post-data "create_db=Create / Reset Database" http://127.0.0.1/setup.php >> /dev/null 2>&1
mysql -u root -e 'update dvwa.users set avatar = "/hackable/users/gordonb.jpg" where user = "gordonb";'
mysql -u root -e 'update dvwa.users set avatar = "/hackable/users/smithy.jpg" where user = "smithy";'
mysql -u root -e 'update dvwa.users set avatar = "/hackable/users/admin.jpg" where user = "admin";'
mysql -u root -e 'update dvwa.users set avatar = "/hackable/users/pablo.jpg" where user = "pablo";'
mysql -u root -e 'update dvwa.users set avatar = "/hackable/users/1337.jpg" where user = "1337";'
echo "Done!"

echo "[*] Starting Firefox to DVWA\nUserName: admin\nPassword: password"
nohup firefox http://127.0.0.1/login.php &> /dev/null &
echo "Done!"
echo "[\033[1;32m*\033[1;37m] DVWA Install Finished!\n"