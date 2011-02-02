require "rubygems"
require "sinatra"
require "ffi"

class Timeval < FFI::Struct
  layout :tv_sec, :long,
         :tv_usec, :long
end # class Timeval

class RUsage < FFI::Struct
  layout :ru_utime, Timeval,
         :ru_stime, Timeval,
         :ru_maxrss, :long,
         :ru_ixrss, :long,
         :ru_idrss, :long,
         :ru_isrss, :long,
         :ru_minflt, :long,
         :ru_majflt, :long,
         :ru_nswap, :long,
         :ru_inblock, :long,
         :ru_oublock, :long,
         :ru_msgsnd, :long,
         :ru_msgrcv, :long,
         :ru_nsignals, :long,
         :ru_nvcsw, :long,
         :ru_nivcsw, :long
end # class RUsage

module LibC
  extend FFI::Library
  ffi_lib FFI::Library::LIBC
  attach_function :getrusage, [:int, :pointer], :int
end

RUSAGE_SELF = 0
rusage = RUsage.new

get "/" do
  LibC::getrusage(RUSAGE_SELF, rusage)
  usertime = rusage[:ru_utime][:tv_sec] + (rusage[:ru_utime][:tv_usec] / 1000000.0)
  systime = rusage[:ru_stime][:tv_sec] + (rusage[:ru_stime][:tv_usec] / 1000000.0)
  [ "User time: #{usertime}",
    "System time: #{systime}",
  ].join("<br>\n")

end

Sinatra::Application.run!
