#!/bin/sh
# Quick installation script for Linux Mint, removes/installs packages would be required by a typical user. 
# Adjusted for Ubuntu Mint 18.1


# Update repositories
sudo apt-get update


# Remove unnecessary components that the installation includes by default
sudo apt-get remove --purge thunderbird pidgin xchat hexchat mintupdate tomboy pix banshee mintinstall rhythmbox -y
sudo apt-get autoremove -y
rm -r ~/Documents ~/Music ~/Videos ~/Public ~/Templates ~/Pictures


# Install 3rd party codecs (necessary for websites and playing video files)
sudo apt-get install adobe-flashplugin gstreamer1.0-plugins-good gstreamer1.0-libav


# Install typical scientific computation tools
if true; then
	sudo apt-get install octave liboctave-dev -y
	sudo apt-get install g++ -y
	sudo apt-get install texlive-base
fi


# Download and install google talk plugin (required for chat/video chat)
if true; then
	wget https://dl.google.com/linux/direct/google-talkplugin_current_amd64.deb
	sudo dpkg -i ~/Downloads/google-talkplugin*
fi


# Download and install google chrome
if false; then
	wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb --tries=1
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


# Set the low/critical battery level to lower percentages than default (works for Ubuntu, not Mint)
if false; then
    gsettings set org.gnome.settings-daemon.plugins.power percentage-low 5
    gsettings set org.gnome.settings-daemon.plugins.power percentage-critical 1
    gsettings set org.gnome.settings-daemon.plugins.power percentage-action 1
    gsettings set org.gnome.settings-daemon.plugins.power use-time-for-policy false
fi


# Reboot
shutdown -r now
