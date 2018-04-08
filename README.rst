usyslogd
========

`usyslogd` is a micro syslog implementation that basically listens to the
syslogd UDP port and transparently calls `syslog(3)`

Install
-------

It is designed to be used with `systemd`.

::

        sudo cp usyslogd.service /etc/systemd/system/
        sudo systemctl enable usyslogd
        sudo systemctl start usyslogd

Configuration
-------------

There is currently *no* configuration possible for the sake of extreme
simplicity & robustness.
