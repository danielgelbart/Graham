class Spdata

  def initinalize(ticker, price, ttm_eps,
                   earnings, shares)
    @ticker = ticker
    @price = price
    @ttm_eps = ttm_eps
    @ttm_earnings = earnings
    @num_shares = shares
  end

  def market_cap
    price * shares
  end


end

