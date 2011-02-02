require "rubygems"
require "sinatra"

get "/" do
  "Hello world!"
end

Sinatra::Application.run!
