#!/bin/sh
# Quick installation script for Linux Mint, removes/installs packages would be required by a typical user. 
# Adjusted for Ubuntu Mint 18.2


# Update repositories
sudo apt-get update


# Remove unnecessary components that the installation includes by default
sudo apt-get remove --purge brasero brasero-cdrkit brasero-common libbrasero-media3-1 -y
sudo apt-get remove --purge thunderbird pidgin xchat hexchat tomboy pix banshee rhythmbox mintupload mintinstall -y
sudo apt-get remove --purge mintupdate
sudo apt-get autoremove -y
rm -r ~/Documents ~/Music ~/Videos ~/Public ~/Templates ~/Pictures

# The corners of the touchpad come with additional functionalities such as copy-paste. Only required for certain laptops with touchpad but not desktops.
if true; then
	echo [Desktop Entry] $'\n' Type=Application $'\n' Exec=synclient RBCornerButton=0 && synclient RTCornerButton=0 $'\n' X-GNOME-Autostart-enabled=true $'\n' NoDisplay=false $'\n' Hidden=false $'\n' Name[en_US]=cornerButtonDisable $'\n' Comment[en_US]= $'\n' X-GNOME-Autostart-Delay=0 >> ~/.config/autostart/cornerButtonDisable.desktop
	synclient RBCornerButton=0
	synclient RTCornerButton=0
fi

# Install 3rd party codecs (necessary for websites and playing video files)
sudo apt-get install adobe-flashplugin -y
sudo apt-get install gstreamer1.0-plugins-bad gstreamer1.0-plugins-good gstreamer1.0-libav -y


# Install typical scientific computation tools
if true; then
	# Install Octave. Second line provides some extra packages Octave complains about during image export.
	sudo apt-get install octave epstool pstoedit transfig -y
	sudo apt-get install g++ -y
	
	# Install Latex compiler
	sudo apt-get install texlive-latex-base -y
fi

# Download and install google chrome
if false; then
	wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb --tries=2
	sudo dpkg -i google-chrome*
	sudo apt-get install -f -y
	rm google*
fi

# Update the system and then remove the unpacked packages
sudo apt-get dist-upgrade -y
sudo apt-get clean


# Generate public encryption key pair (necessary for server authentications)
if true; then
	ssh-keygen -t rsa -b 4096 <<- EOF
	
	
	
	EOF
fi


# Reboot
shutdown -r now
