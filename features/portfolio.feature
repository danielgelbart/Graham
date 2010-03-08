Feature: Managing a user's portfolio

  In order to manage my portfolio
  As a user
  I want to be able to buy and sell stocks

Background:
    Given I have a portfolio

Scenario: Buying a new stock
    When I am on my portfolio page
    And I follow "Add a new stock"

    Then I should see ""
    And I should

Scenario: Buying more of an existing stock


Scenario: Selling a stock


Scenario: Selling some stock
