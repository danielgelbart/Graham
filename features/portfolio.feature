@port
Feature: Managing a user's portfolio

  In order to manage my portfolioas
  As a user
  I want to be able to buy and sell stocks

Background:
    Given I am a user
    Given I have a portfolio called "A"

Scenario: Buying a new stock
    When I am on my portfolio page
    And I follow "Add a new stock"

    And I select a date of purchase
    And I fill in "Comission" with "10"
    And I select "BRK" from "Stock"
    And I fill in "Shares" with "100"
    And I fill in "Price" with "80"
    And I press "Add purchase details"

    Then I should see "Added 100 shares of BRK to your portfolio"
    And I should see "100" and "BRK" displayed
    And I should own "100" shares of "BRK"

    #Buy more shares of the same stock

    When I follow "Add a new stock"

    And I select a date of purchase
    And I fill in "Comission" with "10"
    And I select "BRK" from "Stock"
    And I fill in "Shares" with "100"
    And I fill in "Price" with "80"
    And I press "Add purchase details"

    Then I should see "Added 100 shares of BRK to your portfolio"
    And I should see "200" and "BRK" displayed
    And I should own "200" shares of "BRK"

    #Buy a second stock

    When I follow "Add a new stock"

    And I select a date of purchase
    And I fill in "Comission" with "10"
    And I select "TEVA" from "Stock"
    And I fill in "Shares" with "99"
    And I fill in "Price" with "65"
    And I press "Add purchase details"

    Then I should see "Added 99 shares of TEVA to your portfolio"
    And I should see "99" and "TEVA" displayed
    And I should own "99" shares of "TEVA"



Scenario: Selling a stock


Scenario: Selling some stock
