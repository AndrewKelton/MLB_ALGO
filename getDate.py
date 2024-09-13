# helper file for getting dates
# simpler than using datetime in every file

from datetime import date, timedelta, datetime
hour = int(datetime.now().hour)

def toString(datetime):
    return str('{:%Y-%m-%d}'.format(datetime))

def getToday():
    if hour <= 16:
        day = date.today() - timedelta(days=1)
        return toString(day)
    return toString(date.today())

def getTomorrow():
    day = date.today() + timedelta(days=1)
    if hour <= 16:
        day = date.today()
    return toString(day)

def getYesterday():
    day = date.today() - timedelta(days=1)
    if hour <= 16:
        day = date.today() - timedelta(days=2)
    return toString(day)