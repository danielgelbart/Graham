require 'test_helper'

class SearchesControllerTest < ActionController::TestCase
  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:searches)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create search" do
    assert_difference('Search.count') do
      post :create, :search => { }
    end

    assert_redirected_to search_path(assigns(:search))
  end

  test "should show search" do
    get :show, :id => searches(:one).to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => searches(:one).to_param
    assert_response :success
  end

  test "should update search" do
    put :update, :id => searches(:one).to_param, :search => { }
    assert_redirected_to search_path(assigns(:search))
  end

  test "should destroy search" do
    assert_difference('Search.count', -1) do
      delete :destroy, :id => searches(:one).to_param
    end

    assert_redirected_to searches_path
  end
end
