# Be sure to restart your server when you modify this file.

# Your secret key for verifying cookie session data integrity.
# If you change this key, all old sessions will become invalid!
# Make sure the secret is at least 30 characters and all random, 
# no regular words or you'll be exposed to dictionary attacks.
ActionController::Base.session = {
  :key         => '_graham_session',
  :secret      => '5ffcd3ce835266e78f8bdd3e359f54f943672da6f98cdde1f7651974dc9dd0e5a27fd2d0a5caa798257c79d075c7ed18eed7432411686d737341e5a1249a2cd9'
}

# Use the database for sessions instead of the cookie-based default,
# which shouldn't be used to store highly confidential information
# (create the session table with "rake db:sessions:create")
# ActionController::Base.session_store = :active_record_store
