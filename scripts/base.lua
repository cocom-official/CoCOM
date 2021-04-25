---- print ----
function print(...)
    local arg = {...};
    local printResult = "";
    for i, v in pairs(arg) do
        printResult = printResult .. tostring(v) .. "\t";
    end
    coLua:print(printResult);
end

---- console ----
function console(s, ...)
    coLua:console(s:format(...));
end

function printf(s, ...)
    console(s, ...);
end

---- log ----
function log_level(l, s, ...)
    coLua:log(l, tostring(s):format(...));
end

function log_info(s, ...)
    log_level(0, s, ...);
end

function log_warning(s, ...)
    log_level(1, s, ...);
end

function log_critical(s, ...)
    log_level(2, s, ...);
end

---- sleep/delay ----
function sleep(t)
    coLua:sleep(t);
end

function msleep(t)
    coLua:msleep(t);
end

function delay(t)
    sleep(t);
end

function mdelay(t)
    msleep(t);
end

---- help ----
function help()
    print("Use help to show this info");
    print("Use console/printf as a C-style print function");
    print("Use log_level/log_info/log_warning/log_critical to show log");
    print("Use sleep/msleep/delay/mdelay to sleep or delay");
end